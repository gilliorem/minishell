/* have an array of keys */

// can we split each arg of the cmd
// `unset`: cmd->argv[0];
// `a b c d`: cmd->argv[1]
// need to split the element in cmd->argv[1]
// no even easier than this.
// since cmd->argv is already a list a I loop over
// so I've wrapped the whole function in a for loop that does the same thing for each var and instead
// of return I break as soon as I've made the unset.
