import os
import json
from pprint import pprint


def varfreqs(input):

    variants = {}
    for e in input['variants']:
        allele_count = e['allele_count']
        sample_id = e['sample_id']
        inserted = e['inserted']
        phase = e['phase']

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

    refseqid = input['meta']['reference']
    result = []
    for zygosity in variants:
        for key, val in variants[zygosity].items():
            ref = ''
            for seq in val:
                if seq == 'coverage':
                    continue

                coverage = val['coverage']

                allele_count = len(val[seq])
                sample_count = len(set(val[seq]))
                allele_freq = f"{allele_count}/{coverage}"
                sample_freq = f"{sample_count}/{coverage}"

                if '_' not in key:
                    var = f"{refseqid}:g.{key}{ref}>{seq}"
                elif seq == ".":
                    var = f"{refseqid}:g.{key}del"
                else:
                    var = f"{refseqid}:g.{key}delins{seq}"

                result.append((var, zygosity, allele_freq, sample_freq))

    return sorted(result)


with open('region.json') as fp:
    input = json.load(fp)

ret = varfreqs(input)
for r in ret:
    print(r[0], r[1], r[2], r[3])
