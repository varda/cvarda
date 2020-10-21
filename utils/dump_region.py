import os
import cvarda.ext as cvarda
from pprint import pprint
import json

class app(object):

    config = {
            'COV_REF_CAPACITY': 10000,
            'COV_TREE_CAPACITY': 134217728,
            'SNV_REF_CAPACITY': 10000,
            'SNV_TREE_CAPACITY': 268435456,
            'MNV_REF_CAPACITY': 10000,
            'MNV_TREE_CAPACITY': 134217728,
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


cov_table = _init_cov_table()
snv_table = _init_snv_table()
#mnv_table = _init_mnv_table()

# checkpoint_dir = "/home/masantcroos/sync/checkpoints"
checkpoint_dir = "/data/varda/checkpoints"

cov_table.read(os.path.join(checkpoint_dir, "cov"))
snv_table.read(os.path.join(checkpoint_dir, "snv"))
#mnv_table.read(os.path.join(checkpoint_dir, "mnv"))


def var2cov(r):
    coverage = {}
    for e in r:
        position = e['position'] 
        if position not in coverage:
            coverage[position] = cov_table.query_stab(ref, position, position)

    return coverage


def dump_region(ref, start, end, filename, coverage=False):
    size = 100000
    output = {'snvs': snv_table.query_region(ref, start, end, size)}
    assert len(output['snvs']) < size

    if coverage:
        output['coverage'] = var2cov(output['snvs'])
        print(f"Dumping {len(output['snvs'])} variant and {len(output['coverage'])} coverage entries ...")
    else:
        print(f"Dumping {len(output['snvs'])} variant entries ...")

    with open(filename, 'w') as fp:
        json.dump(output, fp, indent=4, sort_keys=True)


ref = 'NC_000011.9'
start = 44112099
end = 44268980

dump_region(ref, start, end, 'region.json', coverage=True)
