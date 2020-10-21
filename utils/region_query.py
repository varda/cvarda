import os
import json
from pprint import pprint


def regions2X(r):
    variants = {}
    for e in r:
        allele_count = e['allele_count'] 
        position = e['position'] 
        inserted = e['inserted'] 
        sample_id = e['sample_id'] 
        phase = e['phase'] 

        if allele_count not in variants:
            variants[allele_count] = {}

        if position not in variants[allele_count]:
            variants[allele_count][position] = {}

        if inserted not in variants[allele_count][position]:
            variants[allele_count][position][inserted] = []

        variants[allele_count][position][inserted].append({'id': sample_id, 'phase': phase})

    return variants


def regions2vars(r):
    variants = {}
    for e in r:
        allele_count = e['allele_count'] 
        position = e['position'] 
        inserted = e['inserted'] 
        sample_id = e['sample_id'] 
        phase = e['phase'] 

        if allele_count not in variants:
            variants[allele_count] = {}

        if position not in variants[allele_count]:
            variants[allele_count][position] = {'coverage': cov_table.query_stab('NC_012920.1', position, position)}

        if inserted not in variants[allele_count][position]:
            variants[allele_count][position][inserted] = 0

        variants[allele_count][position][inserted] += 1

    return variants


def varfreqs(input):
    refseqid = input['meta']['reference']

    snv_variants = {}
    mnv_variants = {}
    result = []

    for e in input['snvs']:
        allele_count = e['allele_count']
        inserted = e['inserted']
        position = str(e['position'])
        sample_id = e['sample_id']

        if allele_count not in snv_variants:
            snv_variants[allele_count] = {}

        if position not in snv_variants[allele_count]:
            try:
                coverage = input['coverage'][position]
            except KeyError as err:
                raise Exception("coverage not found for SNV", e)

            snv_variants[allele_count][position] = {'coverage': coverage}

        if inserted not in snv_variants[allele_count][position]:
            snv_variants[allele_count][position][inserted] = []

        snv_variants[allele_count][position][inserted].extend(allele_count * [sample_id])

    pprint(snv_variants)

    for e in input['mnvs']:
        allele_count = e['allele_count']
        inserted = e['inserted']
        position = f"{e['start']}_{e['end']}"
        sample_id = e['sample_id']

        if allele_count not in mnv_variants:
            mnv_variants[allele_count] = {}

        if position not in mnv_variants[allele_count]:
            try:
                coverage = input['coverage'][position]
            except KeyError as err:
                raise Exception("coverage not found for MNV", e)

            mnv_variants[allele_count][position] = {'coverage': coverage}

        if inserted not in mnv_variants[allele_count][position]:
            mnv_variants[allele_count][position][inserted] = []

        mnv_variants[allele_count][position][inserted].extend(allele_count * [sample_id])

    pprint(mnv_variants)

    for ac in snv_variants:
        for key, val in snv_variants[ac].items():
            ref = ''
            for nuc in val:
                if nuc == 'coverage':
                    continue

                coverage = val['coverage']

                allele_count = len(val[nuc])
                sample_count = len(set(val[nuc]))
                allele_freq = allele_count / coverage
                sample_freq = sample_count / coverage

                var = f"{refseqid}:g.{key}{ref}>{nuc}"
                result.append((var, allele_freq, sample_freq))

    for ac in mnv_variants:
        for key, val in mnv_variants[ac].items():
            for seq in val:
                if seq == 'coverage':
                    continue

                coverage = val['coverage']

                allele_count = len(val[seq])
                sample_count = len(set(val[seq]))
                allele_freq = allele_count / coverage
                sample_freq = sample_count / coverage

                if seq == ".":
                    var = f"{refseqid}:g.{key}del"
                else:
                    var = f"{refseqid}:g.{key}delins{seq}"
                result.append((var, allele_freq, sample_freq))

    return result


with open('region.json') as fp:
    input = json.load(fp)

# x = regions2X(region)
# pprint(x)

# vars = regions2vars(input['snvs'])
# print(vars)

ret = varfreqs(input)
for r in ret:
    print(r)