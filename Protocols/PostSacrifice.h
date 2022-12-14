/*
 * PostSacrifice.h
 *
 */

#ifndef PROTOCOLS_POSTSACRIFICE_H_
#define PROTOCOLS_POSTSACRIFICE_H_

#include "Protocols/Replicated.h"

template<class T>
class PostSacrifice : public ProtocolBase<T>
{
    typedef typename T::prep_type prep_type;

    typename prep_type::Honest::Protocol internal;
    typename T::Honest::Protocol randomizer;

    vector<array<prep_type, 2>> operands;
    vector<prep_type> results;

    void check();

public:
    Player& P;

    PostSacrifice(Player& P);
    ~PostSacrifice();

    void init_mul(SubProcessor<T>* proc);
    typename T::clear prepare_mul(const T& x, const T& y, int n = -1);
    void exchange() { internal.exchange(); }
    T finalize_mul(int n = -1);

    int get_n_relevant_players() { return internal.get_n_relevant_players(); }
};

#endif /* PROTOCOLS_POSTSACRIFICE_H_ */
