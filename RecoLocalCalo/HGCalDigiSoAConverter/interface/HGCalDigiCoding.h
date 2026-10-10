#ifndef RecoLocalCalo_HGCalDigiSoAConverter_HGCalDigiCoding_h
#define RecoLocalCalo_HGCalDigiSoAConverter_HGCalDigiCoding_h

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <vector>

#include "DataFormats/DetId/interface/DetId.h"
#include "FWCore/Utilities/interface/Exception.h"

// Lossless coding of the in-time samples of one detector instance into a single range-coded byte stream.
//
// The stream starts with the number of digis (4 bytes, little endian), followed by the range-coded bits.
// The geometry's sorted valid-DetId list is walked in groups of cells sharing the DetId bits above
// `groupShift` (a silicon wafer, or a scintillator ring). For each group a "group has digis" bit is coded;
// for occupied groups only, each cell gets a "cell has a digi" bit, and each digi then gets its 12-bit data,
// mode, threshold and ToA-valid bits, and its 10-bit ToA if valid.
// Every bit is coded with an adaptive probability from the counts of the previous bits in the same context:
//  - group bit: all previous groups;
//  - cell bit: the previous cells of the same group;
//  - mode, data: a binary tree over the 13-bit value (mode, data), one context per tree node and per
//    DetId bits 27:26 (the wafer type, i.e. sensor thickness, for silicon);
//  - threshold: one context per mode;
//  - ToA-valid: one context per mode and data / 8, because the ToA is valid only above some amplitude;
//  - ToA: a binary tree over the 10 bits.
// Only integer arithmetic is used, so decoding is exact on any platform.
namespace hgcaldigi::coding {
  struct Sample {
    uint16_t data;  // 12 bits: ADC if mode is 0, ToT otherwise
    bool mode;
    bool threshold;
    bool toaValid;
    uint16_t toa;  // 10 bits, meaningful only if toaValid
  };

  namespace detail {
    // Adaptive probability of a bit from the Krichevsky-Trofimov estimate (2 ones + 1) / (2 seen + 2).
    struct Counts {
      uint32_t ones = 0;
      uint32_t seen = 0;

      // Probability of a 0, in 16 bits.
      uint32_t probabilityOfZero() const {
        uint32_t const ofOne = static_cast<uint32_t>((uint64_t(2 * ones + 1) << 16) / (2 * uint64_t(seen) + 2));
        return std::clamp<uint32_t>(65536 - ofOne, 32, 65536 - 32);
      }

      void update(bool bit) {
        ones += bit;
        ++seen;
      }
    };

    // Binary range coder as in LZMA.
    class Encoder {
    public:
      void encode(bool bit, Counts& counts) {
        uint32_t const bound = (range_ >> 16) * counts.probabilityOfZero();
        counts.update(bit);
        if (bit) {
          low_ += bound;
          range_ -= bound;
        } else {
          range_ = bound;
        }
        while (range_ < (1u << 24)) {
          range_ <<= 8;
          shiftLow();
        }
      }

      void finish() {
        for (int i = 0; i < 5; ++i)
          shiftLow();
      }

      std::vector<uint8_t>& output() { return output_; }

    private:
      void shiftLow() {
        if (static_cast<uint32_t>(low_) < 0xff000000u || (low_ >> 32) != 0) {
          uint8_t const carry = low_ >> 32;
          uint8_t pending = cache_;
          do {
            output_.push_back(pending + carry);
            pending = 0xff;
          } while (--cacheSize_ != 0);
          cache_ = static_cast<uint8_t>(low_ >> 24);
        }
        ++cacheSize_;
        low_ = (low_ & 0x00ffffff) << 8;
      }

      uint64_t low_ = 0;
      uint32_t range_ = 0xffffffff;
      uint8_t cache_ = 0;
      uint64_t cacheSize_ = 1;
      std::vector<uint8_t> output_;
    };

    class Decoder {
    public:
      Decoder(std::vector<uint8_t> const& input, std::size_t position) : input_(input), position_(position) {
        for (int i = 0; i < 5; ++i)
          code_ = (code_ << 8) | next();
      }

      bool decode(Counts& counts) {
        uint32_t const bound = (range_ >> 16) * counts.probabilityOfZero();
        bool const bit = code_ >= bound;
        counts.update(bit);
        if (bit) {
          code_ -= bound;
          range_ -= bound;
        } else {
          range_ = bound;
        }
        while (range_ < (1u << 24)) {
          range_ <<= 8;
          code_ = (code_ << 8) | next();
        }
        return bit;
      }

    private:
      uint8_t next() { return position_ < input_.size() ? input_[position_++] : 0; }

      std::vector<uint8_t> const& input_;
      std::size_t position_;
      uint32_t range_ = 0xffffffff;
      uint32_t code_ = 0;
    };

    constexpr unsigned valueBits = 13;  // mode and 12-bit data
    constexpr unsigned toaBits = 10;

    // Adaptive contexts of one instance; Coder is Encoder or Decoder.
    template <typename Coder>
    class Model {
    public:
      explicit Model(Coder& coder) : coder_(coder) {}

      bool group(bool bit = false) { return code(bit, group_); }
      void startGroup() { cell_ = Counts{}; }
      bool cell(bool bit = false) { return code(bit, cell_); }

      Sample sample(DetId id, Sample const& in = {}) {
        Sample out;
        uint16_t const value = tree(valueTree_[(id.rawId() >> 26) & 0x3], valueBits, (in.mode ? 1u << 12 : 0) | in.data);
        out.mode = value >> 12;
        out.data = value & 0xfff;
        out.threshold = code(in.threshold, threshold_[out.mode]);
        out.toaValid = code(in.toaValid, toaValid_[value >> 3]);
        out.toa = out.toaValid ? tree(toaTree_, toaBits, in.toa) : 0;
        return out;
      }

    private:
      bool code(bool bit, Counts& counts) {
        if constexpr (std::is_same_v<Coder, Encoder>) {
          coder_.encode(bit, counts);
          return bit;
        } else {
          return coder_.decode(counts);
        }
      }

      // Codes the `bits` low bits of value, most significant first, with one context per tree node.
      template <std::size_t N>
      uint16_t tree(std::array<Counts, N>& nodes, unsigned bits, uint16_t value) {
        unsigned node = 1;
        for (unsigned i = bits; i-- > 0;)
          node = 2 * node + code((value >> i) & 1, nodes[node]);
        return node - (1u << bits);
      }

      Coder& coder_;
      Counts group_, cell_;
      std::array<Counts, 2> threshold_;
      std::array<Counts, (1u << valueBits) / 8> toaValid_;
      std::array<std::array<Counts, 1u << valueBits>, 4> valueTree_;
      std::array<Counts, 1u << toaBits> toaTree_;
    };

    // Calls code(groupBegin, groupEnd) for each run of cells sharing the DetId bits above groupShift.
    template <typename F>
    void forEachGroup(std::vector<DetId> const& cells, unsigned groupShift, F&& code) {
      std::size_t begin = 0;
      while (begin < cells.size()) {
        auto const group = cells[begin].rawId() >> groupShift;
        std::size_t end = begin + 1;
        while (end < cells.size() && (cells[end].rawId() >> groupShift) == group)
          ++end;
        code(begin, end);
        begin = end;
      }
    }
  }  // namespace detail

  // Encodes the digis at the strictly increasing positions `occupied` of `cells`, with in-time samples `samples`.
  inline std::vector<uint8_t> encode(std::vector<std::size_t> const& occupied,
                                     std::vector<Sample> const& samples,
                                     std::vector<DetId> const& cells,
                                     unsigned groupShift) {
    detail::Encoder encoder;
    auto& output = encoder.output();
    uint32_t const count = occupied.size();
    for (int i = 0; i < 4; ++i)
      output.push_back(count >> (8 * i));

    auto model = std::make_unique<detail::Model<detail::Encoder>>(encoder);
    std::size_t next = 0;
    detail::forEachGroup(cells, groupShift, [&](std::size_t begin, std::size_t end) {
      if (!model->group(next < occupied.size() && occupied[next] < end))
        return;
      model->startGroup();
      for (std::size_t cell = begin; cell < end; ++cell) {
        bool const hit = next < occupied.size() && occupied[next] == cell;
        model->cell(hit);
        if (hit)
          model->sample(cells[cell], samples[next++]);
      }
    });
    if (next != occupied.size())
      throw cms::Exception("HGCalDigiCoding") << "Digi positions are not increasing cell positions";
    encoder.finish();
    return std::move(output);
  }

  // Decodes the positions, in `cells`, and the in-time samples of the digis.
  inline void decode(std::vector<uint8_t> const& stream,
                     std::vector<DetId> const& cells,
                     unsigned groupShift,
                     std::vector<std::size_t>& occupied,
                     std::vector<Sample>& samples) {
    if (stream.size() < 4)
      throw cms::Exception("CorruptHGCalDigiStream") << "Stream is shorter than its header";
    uint32_t count = 0;
    for (int i = 0; i < 4; ++i)
      count |= uint32_t(stream[i]) << (8 * i);
    occupied.clear();
    samples.clear();
    occupied.reserve(count);
    samples.reserve(count);

    detail::Decoder decoder(stream, 4);
    auto model = std::make_unique<detail::Model<detail::Decoder>>(decoder);
    detail::forEachGroup(cells, groupShift, [&](std::size_t begin, std::size_t end) {
      if (!model->group())
        return;
      model->startGroup();
      for (std::size_t cell = begin; cell < end; ++cell) {
        if (model->cell()) {
          occupied.push_back(cell);
          samples.push_back(model->sample(cells[cell]));
        }
      }
    });
    if (occupied.size() != count)
      throw cms::Exception("CorruptHGCalDigiStream")
          << "Decoded " << occupied.size() << " digis, the stream header says " << count;
  }
}  // namespace hgcaldigi::coding

#endif
