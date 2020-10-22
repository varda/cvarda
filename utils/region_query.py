import os
import json
from pprint import pprint


def varfreqs(input):

    #
    # Group variants by zygosity and sample id
    #
    variants = {}
    for e in input['variants']:
        allele_count = e['allele_count']
        sample_id = e['sample_id']
        inserted = e['inserted']

        # Switch for SNV/MNV
        try:
            position = str(e['position'])
        except KeyError:
            position = f"{e['start']}_{e['end']}"

        if allele_count not in variants:
            variants[allele_count] = {}

        if position not in variants[allele_count]:
            try:
                coverage = input['coverage'][position]
            except KeyError:
                raise Exception("coverage not found for variant", e)

            variants[allele_count][position] = {'coverage': coverage}

        if inserted not in variants[allele_count][position]:
            variants[allele_count][position][inserted] = []

        variants[allele_count][position][inserted].extend(allele_count * [sample_id])

    #
    # Extract frequencies and output as HGVS
    #
    refseqid = input['meta']['reference']
    result = []
    for zygosity in variants:
        for key, val in variants[zygosity].items():
            # TODO: lookup real reference nucleotide
            ref = ''
            for seq in val:
                if seq == 'coverage':
                    continue

                allele_count = len(val[seq])
                allele_coverage = val['coverage']
                sample_count = len(set(val[seq]))
                sample_coverage = int(allele_coverage / 2)  # TODO: sample coverage should be data driven

                # TODO: are these conditionals fool proof?
                if '_' not in key:
                    var = f"{refseqid}:g.{key}{ref}>{seq}"
                elif seq == ".":
                    var = f"{refseqid}:g.{key}del"
                else:
                    var = f"{refseqid}:g.{key}delins{seq}"

                result.append((var, zygosity, allele_count, allele_coverage, sample_count, sample_coverage))

    return sorted(result)


with open('region.json') as fp:
    input = json.load(fp)

ret = varfreqs(input)
print("var, zygosity, allele_count, allele_coverage, sample_count, sample_coverage")
for r in ret:
    print(','.join(str(x) for x in r))
