import cvarda


def test_mnv_common_end():
    mnv_table = cvarda.MNVTable()
    seq_table = cvarda.SequenceTable()

    index = seq_table.insert("")

    mnv_table.insert("chr1", 1, 4, 1, 1, index, 1)
    mnv_table.insert("chr1", 2, 4, 1, 1, index, 2)

    assert mnv_table.diagnostics() == \
        {'chr1': {'height': 2, 'entry_size': 32, 'entries': 2}}

    assert mnv_table.query("chr1", 1, 4, index) == 1

    assert mnv_table.query("chr1", 2, 4, index) == 1

    assert mnv_table.query("chr1", 3, 4, index) == 0
