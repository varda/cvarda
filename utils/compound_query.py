import os
import json
from pprint import pprint


def groupby_sample_and_phase(input):

    #
    # Group variants by zygosity and sample id
    #
    variants = {}
    for e in input['variants']:
        allele_count = e['allele_count']
        sample_id = e['sample_id']
        inserted = e['inserted']
        phase = e['phase']

        # Switch for SNV/MNV
        try:
            position = str(e['position'])
        except KeyError:
            position = f"{e['start']}_{e['end']}"

        if sample_id not in variants:
            variants[sample_id] = {}

        if allele_count not in variants[sample_id]:
            variants[sample_id][allele_count] = {}

        if phase not in variants[sample_id][allele_count]:
            variants[sample_id][allele_count][phase] = {}

        variants[sample_id][allele_count][phase][position] = inserted

    return variants


def find_compound(input, meta):
    ref = ''
    refseqid = meta['reference']

    output = {}

    for sample, data in input.items():
        output[sample] = {
            'phased': [],
            'unphased': [],
            'homozygous': [],
        }

        # Loop over allele_count, but not really use the value here.
        # It's important to store it based on the allele_count though,
        # because in that way we can store both homozygous and heterozygous
        # variants on the same position independently.
        for allele_count, data2 in data.items():
            for phase, vars in data2.items():
                v = []
                for pos, seq in vars.items():

                    # TODO: are these conditionals fool proof?
                    if '_' not in pos:
                        var = f"{refseqid}:g.{pos}{ref}>{seq}"
                    elif seq == ".":
                        var = f"{refseqid}:g.{pos}del"
                    else:
                        var = f"{refseqid}:g.{pos}delins{seq}"
                    v.append(var)

                # Multiple variants with the same phase id are considered in phase
                if phase > 0 and len(v) > 1:
                    output[sample]['phased'].append(tuple(v))
                # Single variants can happen because of how GATK reports them.
                # In VCF homozygous variants also get a phase set id, we threat them all as -1
                elif phase > 0 and len(v) <= 1:
                    output[sample]['unphased'].extend(v)
                # Unphased heterozygous variant
                elif phase == 0:
                    output[sample]['unphased'].extend(v)
                # "Phased" homozygous variant
                else:
                    output[sample]['homozygous'].extend(v)

    return output


with open('region.json') as fp:
    input = json.load(fp)

ret = groupby_sample_and_phase(input)
pprint(ret)

ret = find_compound(ret, input['meta'])
pprint(ret)
