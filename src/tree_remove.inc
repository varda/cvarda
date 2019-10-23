static size_t
tree_to_vine(TREE* const tree)
{
    uint32_t par = tree->root;
    uint32_t tmp = tree->root;
    size_t count = 0;
    while (NULLPTR != tmp)
    {
        if (NULLPTR == tree->nodes[tmp].child[RIGHT])
        {
            par = tmp;
            tmp = tree->nodes[tmp].child[LEFT];
            count += 1;
        } // if
        else
        {
            uint32_t const right = tree->nodes[tmp].child[RIGHT];
            tree->nodes[tmp].child[RIGHT] = tree->nodes[right].child[LEFT];
            tree->nodes[right].child[LEFT] = tmp;
            if (tmp == tree->root)
            {
                tree->root = right;
                par = right;
            } // if
            else
            {
                tree->nodes[par].child[LEFT] = right;
            } // else
            tmp = right;
        } // else
    } // while
    return count;
} // tree_to_vine


static inline int
ilog2(size_t const x)
{
    return 64 - __builtin_clzll(x);
} // ilog2


static inline size_t
ipow2(int const x)
{
    return (1ull << x);
} // ipow2


static void
compress(TREE* const tree, size_t const count)
{
    uint32_t tmp = tree->nodes[tree->root].child[LEFT];
    tree->nodes[tree->root].child[LEFT] = tree->nodes[tmp].child[RIGHT];
    tree->nodes[tmp].child[RIGHT] = tree->root;
    tree->root = tmp;

    tmp = tree->root;
    for (size_t i = 1; i < count; ++i)
    {
        uint32_t const p = tree->nodes[tmp].child[LEFT];
        uint32_t const q = tree->nodes[p].child[LEFT];

        tree->nodes[p].child[LEFT] = tree->nodes[q].child[RIGHT];
        tree->nodes[q].child[RIGHT] = p;
        tree->nodes[tmp].child[LEFT] = q;
        tmp = q;
    } // for
} // compress


// Day Stout Warren algorithm
static void
balance(TREE* const tree)
{
    size_t const nodes = tree_to_vine(tree);

    size_t const leaves = nodes - (ipow2(ilog2(nodes + 1) - 1) - 1);
    if (0 < leaves)
    {
        compress(tree, leaves);
    } // if

    size_t rem = nodes - leaves;
    while (1 < rem)
    {
        rem /= 2;
        compress(tree, rem);
    } // while
} // balance


// Adapted from:
// http://adtinfo.org/libavl.html/Deleting-from-an-AVL-Tree.html
static void
remove_node(TREE* const tree,
            int const depth,
            uint64_t const path)
{
    uint32_t nodes[64] = {NULLPTR};
    int dir[64] = {LEFT};
    uint32_t tmp = tree->root;
    for (int i = 0; i < depth; ++i)
    {
        nodes[i] = tmp;
        dir[i] = ((path >> (depth - i - 1)) & RIGHT);
        tmp = tree->nodes[tmp].child[((path >> (depth - i - 1)) & RIGHT)];
    } // for

    int len = depth;

    if (NULLPTR == tree->nodes[tmp].child[RIGHT])
    {
        if (0 == depth)
        {
            tree->root = tree->nodes[tmp].child[LEFT];
        } // if
        else
        {
            tree->nodes[nodes[depth - 1]].child[dir[depth - 1]] = tree->nodes[tmp].child[LEFT];
        } // else
    } // if
    else
    {
        uint32_t right = tree->nodes[tmp].child[RIGHT]; // right sub tree

        if (NULLPTR == tree->nodes[right].child[LEFT])
        {
            tree->nodes[right].child[LEFT] = tree->nodes[tmp].child[LEFT];
            tree->nodes[right].balance = tree->nodes[tmp].balance;

            if (0 == depth)
            {
                tree->root = right;
            } // if
            else
            {
                tree->nodes[nodes[depth - 1]].child[dir[depth - 1]] = right;
            } // else
            dir[len] = RIGHT;
            nodes[len] = right;
            len += 1;
        } // if
        else
        {
            uint32_t suc = NULLPTR; // find the inorder successor

            len += 1;
            while (1)
            {
                dir[len] = LEFT;
                nodes[len] = right;
                len += 1;
                suc = tree->nodes[right].child[LEFT];
                if (NULLPTR == tree->nodes[suc].child[LEFT])
                {
                    break;
                } // if
                right = suc;
            } // while

            tree->nodes[suc].child[LEFT] = tree->nodes[tmp].child[LEFT];
            tree->nodes[right].child[LEFT] = tree->nodes[suc].child[RIGHT];
            tree->nodes[suc].child[RIGHT] = tree->nodes[tmp].child[RIGHT];
            tree->nodes[suc].balance = tree->nodes[tmp].balance;

            if (0 == depth)
            {
                tree->root = suc;
            } // if
            else
            {
                tree->nodes[nodes[depth - 1]].child[dir[depth - 1]] = suc;
            } // else
            dir[depth] = RIGHT;
            nodes[depth] = suc;
        } // else
    } // else
} // remove_node


static size_t
traverse(TREE* const restrict tree,
         uint32_t const root,
         int const depth,
         uint64_t const path,
         vrd_AVL_Tree const* const restrict subset)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    size_t count = 0;

    count += traverse(tree, tree->nodes[root].child[LEFT], depth + 1, (path << 1) + LEFT, subset);
    count += traverse(tree, tree->nodes[root].child[RIGHT], depth + 1, (path << 1) + RIGHT, subset);

    if (vrd_avl_tree_is_element(subset, tree->nodes[root].sample_id))
    {
        remove_node(tree, depth, path);
        count += 1;
    } // if
    return count;
} // traverse


#ifdef ITV
static int
update_avl(TREE* const tree, uint32_t const root, uint32_t* const new_max)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    uint32_t max_left = 0;
    int const left = update_avl(tree, tree->nodes[root].child[LEFT], &max_left);
    uint32_t max_right = 0;
    int const right = update_avl(tree, tree->nodes[root].child[RIGHT], &max_right);
    tree->nodes[root].max = max(max(max_left, max_right), tree->nodes[root].end);
    tree->nodes[root].balance = right - left;
    *new_max = tree->nodes[root].max;
    return 1 + max(left, right);
} // update_avl
#else
static int
update_avl(TREE* const tree, uint32_t const root)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    int const left = update_avl(tree, tree->nodes[root].child[LEFT]);
    int const right = update_avl(tree, tree->nodes[root].child[RIGHT]);
    tree->nodes[root].balance = right - left;
    return 1 + max(left, right);
} // update_avl
#endif
