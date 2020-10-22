import os
import cvarda.ext as cvarda
import json

class app(object):

    config = {
            'COV_REF_CAPACITY': 10000,
            'COV_TREE_CAPACITY': 134217728,
            'SNV_REF_CAPACITY': 10000,
            'SNV_TREE_CAPACITY': 268435456,
            'MNV_REF_CAPACITY': 10000,
            'MNV_TREE_CAPACITY': 134217728,
            'SEQ_CAPACITY': 10000000,
    }


def _init_cov_table():
    ref_capacity = app.config["COV_REF_CAPACITY"]
    tree_capacity = app.config["COV_TREE_CAPACITY"]
    table = cvarda.CoverageTable(ref_capacity, tree_capacity)
    return table


def _init_snv_table():
    ref_capacity = app.config["SNV_REF_CAPACITY"]
    tree_capacity = app.config["SNV_TREE_CAPACITY"]
    table = cvarda.SNVTable(ref_capacity, tree_capacity)
    return table


def _init_mnv_table():
    ref_capacity = app.config["MNV_REF_CAPACITY"]
    tree_capacity = app.config["MNV_TREE_CAPACITY"]
    table = cvarda.MNVTable(ref_capacity, tree_capacity)
    return table


def _init_seq_table():
    seq_capacity = app.config["SEQ_CAPACITY"]
    table = cvarda.SequenceTable(seq_capacity)
    return table


cov_table = _init_cov_table()
snv_table = _init_snv_table()
mnv_table = _init_mnv_table()
seq_table = _init_seq_table()

# checkpoint_dir = "/home/masantcroos/sync/checkpoints"
checkpoint_dir = "/data/varda/checkpoints"

cov_table.read(os.path.join(checkpoint_dir, "cov"))
snv_table.read(os.path.join(checkpoint_dir, "snv"))
mnv_table.read(os.path.join(checkpoint_dir, "mnv"))
seq_table.read(os.path.join(checkpoint_dir, "seq"))


def var2cov(snvs, mnvs):
    coverage = {}

    for e in snvs:
        position = e['position']
        if position not in coverage:
            coverage[str(position)] = cov_table.query_stab(ref, position, position)

    for e in mnvs:
        start = e['start']
        end = e['end']
        key = f'{start}_{end}'
        if key not in coverage:
            coverage[key] = cov_table.query_stab(ref, start, end)

    return coverage


def dump_region(ref_id, start, end, filename, coverage=False):
    size = 100000
    output = {'meta': {'reference': ref_id, 'start': start, 'end': end}}
    snv_variants = snv_table.query_region(ref, start, end, size)
    mnv_variants  = mnv_table.query_region(ref, start, end, size, seq_table)
    assert len(snv_variants) < size
    assert len(mnv_variants) < size

    output['variants'] = snv_variants + mnv_variants

    if coverage:
        output['coverage'] = var2cov(snv_variants, mnv_variants)
        print(f"Dumping {len(output['variants'])} variant and {len(output['coverage'])} coverage entries ...")
    else:
        print(f"Dumping {len(output['variants'])} variant entries ...")

    with open(filename, 'w') as fp:
        json.dump(output, fp, indent=4, sort_keys=True)


ref = 'NC_000011.9'
start = 44112099
end = 44268980

dump_region(ref, start, end, 'region.json', coverage=True)
