import cvarda


def test_diag_cov_entries():
    cov_table = cvarda.CoverageTable()

    diag = cov_table.diagnostics()
    assert diag == {}

    cov_table.insert('chr1', 10, 11, 0)
    diag = cov_table.diagnostics()
    assert diag == {'chr1': {'height': 1, 'entries': 1}}

    cov_table.insert('chr1', 5, 6, 0)
    diag = cov_table.diagnostics()
    assert diag == {'chr1': {'height': 2, 'entries': 2}}

    cov_table.insert('chr1', 15, 16, 0)
    diag = cov_table.diagnostics()
    assert diag == {'chr1': {'height': 2, 'entries': 3}}

    cov_table.insert('chr1', 4, 5, 0)
    diag = cov_table.diagnostics()
    assert diag == {'chr1': {'height': 3, 'entries': 4}}


def test_diag_cov_file():
    ref_capacity = 1000  # max number of distinct reference sequences
    tree_capacity = pow(2, 24)  # number of nodes per tree
    cov_table = cvarda.CoverageTable(ref_capacity, tree_capacity)

    coverage_filename = 'python_ext/tests/test_diag_coverage.varda'
    cvarda.coverage_from_file(coverage_filename, 1, cov_table)
    stats = cov_table.diagnostics()
    assert len(stats) == 83
