/*
 * RuntimeBranching.h
 *
 */

#ifndef GC_RUNTIMEBRANCHING_H_
#define GC_RUNTIMEBRANCHING_H_

namespace GC
{

class RuntimeBranching
{
    bool tainted;

public:
	RuntimeBranching() : tainted(false)
	{
	}

    void untaint()
    {
        bool was_tainted = tainted;
        tainted = false;
        if (was_tainted)
            throw needs_cleaning();
    }
    void taint()
    {
        tainted = true;
    }
};

} /* namespace GC */

#endif /* GC_RUNTIMEBRANCHING_H_ */
