#include "lwe.h"
#include "ringlwe.h"
#include "ringgsw.h"
#include "multiplication.h"



RingGSWParams::RingGSWParams(int l, int Bgbit, RingLWEParams* ringlwe_params):
    l(l),
    Bgbit(Bgbit),
    Bg(1<<Bgbit),
    halfBg(Bg/2),
    maskMod(Bg-1),
    ringlwe_params(ringlwe_params),
    kpl(int((ringlwe_params->k+1)*l))
    {
        h = new Torus32[l];
        for (int i = 0; i < l; ++i) {
    	    int kk = (32-(i+1)*Bgbit);
    	    h[i] = 1 << kk; // 1/(Bg^(i+1)) as a Torus32
	    }	

        // offset = Bg/2 * (1 + Bg + Bg^2 + ... + Bg^(l-1)) 
        Torus32 offset;
        Torus32 temp0;
        offset = 0;
        for (int i = 0; i < l; ++i) {
            temp0 = offset;
            offset = temp0*Bg + halfBg;
        }

    }


RingGSWParams::~RingGSWParams() {
    delete[] h;
}



// same key as in RingLWE
RingGSWKey::RingGSWKey(const RingGSWParams* params):
    params(params)
{
    key = new_IntPolynomial_array(params->ringlwe_params->k,params->ringlwe_params->N);   
}

RingGSWKey::~RingGSWKey() {
    delete_IntPolynomial_array(params->ringlwe_params->k,key);
}



RingGSWSample::RingGSWSample(const RingGSWParams* params): k(params->ringlwe_params->k), l(params->l) {
    all_sample = new_RingLWESample_array((k+1)*l,params->ringlwe_params); // tous les samples comme un vecteur ligne
    row_sample = new RingLWESample*[(k+1)*l]; // lignes de la matrice RingGSW
    bloc_sample = new RingLWESample*[k+1]; // blocs horizontaux (l lignes) de la matrice RingGSW

    for (int p = 0; p < (k+1)*l; ++p)
        row_sample[p] = all_sample + p;
    for (int p = 0; p < k+1; ++p)
	    bloc_sample[p] = all_sample + p*l;

	current_alpha = 0;
}

RingGSWSample::~RingGSWSample() {
    delete_RingLWESample_array((k+1)*l,all_sample);
    delete[] row_sample;
    delete[] bloc_sample;
}

RingGSWSampleFFT::RingGSWSampleFFT(const RingGSWParams* params): k(params->ringlwe_params->k), l(params->l) {
    all_samples = new_RingLWESampleFFT_array((k+1)*l,params->ringlwe_params);
    sample = new RingLWESampleFFT*[(k+1)*l];

    for (int p = 0; p < (k+1); ++p)
	sample[p] = all_samples + p*l;

    current_alpha = 0;
}

RingGSWSampleFFT::~RingGSWSampleFFT() {
    delete_RingLWESampleFFT_array((k+1)*l,all_samples);
    delete[] sample;
}


