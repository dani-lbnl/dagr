#ifndef dagr_algorithm_output_port_h
#define dagr_algorithm_output_port_h

using dagr_algorithm_output_port
    = std::pair<p_dagr_algorithm, unsigned int>;

// convenience functions for accessing port and algorithm
// from an output port
inline
p_dagr_algorithm &get_algorithm(dagr_algorithm_output_port &op)
{ return op.first; }

inline
unsigned int &get_port(dagr_algorithm_output_port &op)
{ return op.second; }

#endif
