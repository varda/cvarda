import cvarda.ext as cvarda


def test_cov_entries():
    cov = cvarda.CoverageTable()
    cov.insert('chr1', 5, 10, 2, 42)
    assert cov.query_stab('chr1', 6, 8) == 2
