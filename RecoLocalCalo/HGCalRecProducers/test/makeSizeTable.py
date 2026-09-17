from pathlib import Path

import pandas as pd

print("""
Remember to update .size files!

edmEventSize -v stepHLT_onlyHGCalCompression_LZMA.root > stepHLT_all_onlyHGCalCompression_LZMA.size 
edmEventSize -v stepHLT_onlyHGCalCompression.root > stepHLT_all_onlyHGCalCompression.size 

""")
def classify_branch(branch_name):
    """Categorizes branches into sections and filters out auxiliary collections."""
    # Exclude intermediate Decompressed / Compressed variants
    if "Decompressed" in branch_name:
        return "Decompressed rechits"
    elif "Compressed" in branch_name:
        return "Compressed rechits"
    elif "simHGCalUnsuppressedDigis" in branch_name:
        return "Simulated unsuppressed digis"
    elif "hltHgcalDigis" in branch_name:
        return "Digis"
    elif "hltHGCalUncalibRecHit_" in branch_name:
        return "Uncalibrated rechits"
    elif "hltHGCalRecHit" in branch_name:
        return "Calibrated rechits"
    elif "recoCaloClusters_hltMergeLayerClusters" in branch_name and "sharing" not in branch_name:
        return "Merged layer clusters"
    elif "recoCaloClusters_hltHgcalLayerClusters" in branch_name:
        return "Layer clusters"
    
    return None

def process_hgcal_sizes(data_str, compressed_column="Compressed"):
    records = []
    
    for line in data_str.strip().split("\n"):
        parts = line.strip().split()
        if len(parts) < 3:
            continue
            
        branch = parts[0].rstrip(".")
        try:
            uncompressed = float(parts[1])
            compressed = float(parts[2])
        except ValueError:
            continue
            
        section = classify_branch(branch)
        if section:
            records.append({
                "Branch": branch,
                "Section": section,
                "Uncompressed": uncompressed,
                compressed_column: compressed
            })

    df = pd.DataFrame(records)
    
    # Section order
    sections = [
        "Simulated unsuppressed digis",
        "Digis",
        "Uncalibrated rechits",
        "Calibrated rechits",
        "Layer clusters",
        "Merged layer clusters",
        "Compressed rechits",
        "Decompressed rechits",
    ]

    # Build Summary Table
    summary_data = []
    for sec in sections:
        sec_df = df[df["Section"] == sec]
        u_sum = sec_df["Uncompressed"].sum()
        c_sum = sec_df[compressed_column].sum()
        summary_data.append({
            "Section": sec,
            "Uncompressed Total [MB]": f"{u_sum / 1e6:.2f}",
            f"{compressed_column} Total [MB]": f"{c_sum / 1e6:.2f}"
        })

    summary_df = pd.DataFrame(summary_data)
    
    return df, summary_df

f_name = Path("stepHLT_all_onlyHGCalCompression.size")
lzma4_f_name = Path("stepHLT_all_onlyHGCalCompression_LZMA.size")

raw_data = f_name.read_text()
lzma4_raw_data = lzma4_f_name.read_text()

# Execute processing
detail_df, summary_df = process_hgcal_sizes(raw_data)
lzma4_detail_df, lzma4_summary_df = process_hgcal_sizes(
    lzma4_raw_data, compressed_column="LZMA4 Compressed"
)

summary_df = summary_df.merge(
    lzma4_summary_df[["Section", "LZMA4 Compressed Total [MB]"]],
    on="Section",
    how="left",
    validate="one_to_one",
)

print("### Section Totals Summary")
print(summary_df.to_markdown(index=False))
