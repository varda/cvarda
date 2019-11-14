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


def test_seq_table_trie():
    seq = cvarda.SequenceTable();

    words = {'romane': None,
             'romanus': None,
             'romulus': None,
             'rubens': None,
             'ruber': None,
             'rubicon': None,
             'rubicundus': None}

    idx = 0
    for key in words:
        words[key] = seq.insert(key)
        words[key] = seq.insert(key)

        assert words[key] == idx
        idx = idx + 1

    for key in words:
        assert seq.remove(words[key]) != 0

    for key in words:
        assert words[key] == seq.query(key)

    for key in words:
        assert seq.remove(words[key]) != 0

    for key in words:
        assert None is seq.query(key)

