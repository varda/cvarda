import cvarda


def test_diag():
    cov_table = cvarda.CoverageTable()

    diag = cov_table.diagnostics()
    assert diag == {}

    cov_table.insert('chr1', 10, 11, 0)
    diag = cov_table.diagnostics()
    assert diag == {'chr1': {'height': 1, 'nodes': 1}}

    cov_table.insert('chr1', 5, 6, 0)
    diag = cov_table.diagnostics()
    assert diag == {'chr1': {'height': 2, 'nodes': 2}}

    cov_table.insert('chr1', 15, 16, 0)
    diag = cov_table.diagnostics()
    assert diag == {'chr1': {'height': 2, 'nodes': 3}}

    cov_table.insert('chr1', 4, 5, 0)
    diag = cov_table.diagnostics()
    assert diag == {'chr1': {'height': 3, 'nodes': 4}}
