import cvarda


def test_seq_table_index():
    seq_table = cvarda.SequenceTable()

    index = seq_table.query('A')
    assert index is None

    index = seq_table.insert('A')
    assert index == 0

    index = seq_table.insert('B')
    assert index == 1

    index = seq_table.insert('C')
    assert index == 2

    index = seq_table.query('D')
    assert index is None

    index = seq_table.query('B')
    assert index == 1

    index = seq_table.query('A')
    assert index == 0

    index = seq_table.query('C')
    assert index == 2
