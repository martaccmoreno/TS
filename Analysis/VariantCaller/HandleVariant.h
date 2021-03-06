/* Copyright (C) 2012 Ion Torrent Systems, Inc. All Rights Reserved */

//! @file     HandleVariant.h
//! @ingroup  VariantCaller
//! @brief    HP Indel detection


#ifndef HANDLEVARIANT_H
#define HANDLEVARIANT_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <ctype.h>
#include <algorithm>
#include "api/api_global.h"
#include "api/BamAux.h"
#include "api/BamConstants.h"
#include "api/BamReader.h"
#include "api/SamHeader.h"
#include "api/BamAlignment.h"
#include "api/SamReadGroup.h"
#include "api/SamReadGroupDictionary.h"
#include "api/SamSequence.h"
#include "api/SamSequenceDictionary.h"

#include "sys/types.h"
#include "sys/stat.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include <list>
#include <map>
#include <Variant.h>

#include "InputStructures.h"

using namespace std;
using namespace BamTools;
using namespace ion;

class MolecularFamily;
class EnsembleEval;

// ----------------------------------------------------------------------
// The function of doing flow-space evaluation for the variant
int EnsembleProcessOneVariant(PersistingThreadObjects &thread_objects, VariantCallerContext& vc,
    VariantCandidate &current_variant, const PositionInProgress& bam_position,
	vector< vector< MolecularFamily> > &molecular_families_one_sample, int sample_index = -1);

// ----------------------------------------------------------------------
// The interface for the candidate generator to exam the candidate variants
class CandidateExaminer{
private:
	PersistingThreadObjects* thread_objects_;
	VariantCallerContext* vc_;
	EnsembleEval* my_ensemble_;
	int max_group_size_allowed_;
	//! @brief The allele preparation steps: setup alleles and filter alleles
	void PrepareAlleles_(VariantCandidate& candidate_variant);

public:
	CandidateExaminer();
	CandidateExaminer(PersistingThreadObjects* thread_objects, VariantCallerContext* vc);
	~CandidateExaminer();
	//! @brief Initialize the object
	void Initialize(PersistingThreadObjects* thread_objects, VariantCallerContext* vc);
	//! @brief Setup variant candidates for examination
	void SetupVariantCandidate(VariantCandidate& candidate_variant);
	//! @brief Clear variant candidates
	void ClearVariantCandidate();
	//! @brief Calculate the 0-based end position of the look ahead window
	int FindLookAheadEnd0(int current_candidate_gen_win_end0 = -1);
	//! @brief Calculate the 1-based end position of the look ahead window
	int FindLookAheadEnd1(int current_candidate_gen_win_end1 = 0);
	//! @brief Split the candidate variant into smaller callable variants
	void SplitCandidateVariant(list<list<int> >& allele_groups);
	//! @brief Calculate the FD of the (reference, alternatives) alleles vs. reads as called on test_read_stack
	void QuickExamFD(vector<const Alignment *>& test_read_stack, vector<vector<int> >& flow_disruptive_code);
	//! @brief The interface for the look ahead "sliding" window in the 0-based coordinate
	void LookAheadSlidingWindow0(list<list<int> >& allele_groups_ready_to_go, vector<int>& alleles_on_hold, int& sliding_window_start_0, int& sliding_window_end_0, int current_candidate_gen_window_end_0);
	//! @brief The interface for the look ahead "sliding" window in the 1-based coordinate
	void LookAheadSlidingWindow1(list<list<int> >& allele_groups_ready_to_go, vector<int>& alleles_on_hold, int& sliding_window_start_1, int& sliding_window_end_1, int current_candidate_gen_window_end_1);
};

//! @brief Check if the allele described by position_1, ref, alt_allele, alt_orig_padding is in variant.
bool IsAlleleInVariant(const vcf::Variant& variant, long position_1, const string& ref, const string& alt_allele, const pair<int, int>& alt_orig_padding = {0, 0});


#endif //HANDLEVARIANT_H
