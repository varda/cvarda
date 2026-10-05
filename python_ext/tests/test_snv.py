import cvarda.ext as cvarda
import pytest


def test_snv_entries():
    snv_table = cvarda.SNVTable()
    mnv_table = cvarda.MNVTable()
    seq_table = cvarda.SequenceTable()

    diag = snv_table.diagnostics()
    assert diag == {}

    snv_table.insert('chr1', 1, 1, 1, "A", 1)
    diag = snv_table.diagnostics()
    assert diag == {'chr1': {'height': 1, 'entry_size': 20, 'entries': 1}}

    snv_table.insert('chr1', 2, 1, 1, "C", 1)
    diag = snv_table.diagnostics()
    assert diag == {'chr1': {'height': 2, 'entry_size': 20, 'entries': 2}}

    variants_filename = 'python_ext/tests/test_variants_small.varda'

    ret = cvarda.variants_from_file(variants_filename, 1, snv_table, mnv_table, seq_table)
    assert ret == 3
    diag = snv_table.diagnostics()
    assert diag == {'chr1': {'height': 3, 'entry_size': 20, 'entries': 4}}

    diag = mnv_table.diagnostics()
    assert diag == {'chr1': {'height': 1, 'entry_size': 32, 'entries': 1}}


def test_snv_query_ref():
    snv_table = cvarda.SNVTable()

    with pytest.raises(ValueError) as excinfo:
        query = snv_table.query('chr1', 1, "A")

    assert "SNVTable.query: reference not found" == str(excinfo.value)

    snv_table.insert('chr1', 1, 1, 1, "A", 1)

    query = snv_table.query('chr1', 1, "A")
    assert query == 1


def test_snv_query_ids():
    snv_table = cvarda.SNVTable()

    snv_table.insert('chr1', 1, 1, 0, "A")
    snv_table.insert('chr1', 1, 1, 1, "A")
    snv_table.insert('chr1', 1, 1, 2, "C")
    snv_table.insert('chr1', 1, 1, 3, "A")

    response = snv_table.query('chr1', 1, "A")
    assert response == 3

    response = snv_table.query('chr1', 1, "A", False, 10)
    assert sorted(response) == [0, 1, 3]
