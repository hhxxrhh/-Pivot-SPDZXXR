/*
 * PrivateOutput.cpp
 *
 */

#include "PrivateOutput.h"
#include "Processor.h"

template<class T>
void PrivateOutput<T>::start(int player, int target, int source)
{
    open_type mask;
    proc.DataF.get_input(proc.get_S_ref(target), mask, player);
    proc.get_S_ref(target) += proc.get_S_ref(source);

    if (player == proc.P.my_num())
        masks.push_back(mask);
}

template<class T>
void PrivateOutput<T>::stop(int player, int source)
{
    if (player == proc.P.my_num())
    {
        open_type value;
        value.sub(proc.get_C_ref(source), masks.front());
        value.output(proc.Proc.private_output, false);
        masks.pop_front();
    }
}
