#ifndef Match_h_
#define Match_h_

const float GenPtThreshold = 20;
int Match(const reco::Candidate & candidate, const edm::View<reco::Candidate> & collection)
{
typedef math::XYZTLorentzVector 	LorentzVector;
int idx=-1;
//const LorentzVector& cand = candidate.p4();
float mindR=999;
for(edm::View<reco::Candidate>::const_iterator it=collection.begin(); it!=collection.end(); it++)
{
	if(it->pt()<GenPtThreshold) continue;
	float dR = reco::deltaR(*it,candidate);
	if(dR<mindR)
	{
		mindR=dR;
		idx=abs(it-collection.begin());
	}
}

if(mindR<0.5) return idx;
else		return -1;
} 

#endif 
