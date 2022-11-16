/*
 * PostSacriRepRingShare.h
 *
 */

#ifndef PROTOCOLS_POSTSACRIREPRINGSHARE_H_
#define PROTOCOLS_POSTSACRIREPRINGSHARE_H_

#include "Protocols/MaliciousRep3Share.h"

template<class T> class MalRepRingPrepWithBits;
template<class T> class PostSacrifice;

template<int K, int S>
class PostSacriRepRingShare : public MaliciousRep3Share<Z2<K>>
{
    typedef MaliciousRep3Share<Z2<K>> super;

public:
    static const int BIT_LENGTH = K;
    static const int SECURITY = S;

    typedef SignedZ2<K> clear;
    typedef MaliciousRep3Share<Z2<K + S>> prep_type;
    typedef Z2<S> random_type;

    typedef PostSacrifice<PostSacriRepRingShare> Protocol;
    typedef HashMaliciousRepMC<PostSacriRepRingShare> MAC_Check;
    typedef MAC_Check Direct_MC;
    typedef ReplicatedInput<PostSacriRepRingShare> Input;
    typedef ::PrivateOutput<PostSacriRepRingShare> PrivateOutput;
    typedef MalRepRingPrepWithBits<PostSacriRepRingShare> LivePrep;

    static string type_short()
    {
        return "PR";
    }

    PostSacriRepRingShare()
    {
    }
    PostSacriRepRingShare(const clear& other, int my_num, clear alphai = {}) :
            super(other, my_num, alphai)
    {
    }
    template<class U>
    PostSacriRepRingShare(const U& other) : super(other)
    {
    }
};

#endif /* PROTOCOLS_POSTSACRIREPRINGSHARE_H_ */
