import cvarda.ext as cvarda


def test_samples_entries():
    snv_table = cvarda.SNVTable()
    mnv_table = cvarda.MNVTable()
    cov_table = cvarda.CoverageTable()
    seq_table = cvarda.SequenceTable()

    # SNV
    # refseq, pos, allele_count, sample_id, inserted, phase_set_id
    sample_id = 0
    snv_table.insert('chr1', 1, 1, sample_id, "A", 1)
    sample_id = 0
    snv_table.insert('chr1', 2, 1, sample_id, "C", 1)

    # MNV
    # refseq, start, end, allele_count, sample_id, sequence, phase_set_id
    seq_index = seq_table.insert("ACTG")
    sample_id = 0
    mnv_table.insert("chr1", 1, 4, 1, sample_id, seq_index, 1)

    # Coverage
    # refseq, start, end, allele_count, sample_id
    sample_id = 0
    cov_table.insert('chr1', 4, 5, 2, sample_id)

    counts = cvarda.sample_count(cov_table, snv_table, mnv_table)

    assert counts == [4]