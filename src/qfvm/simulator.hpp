#pragma once

#include "statevector.hpp"
#include "circuit.hpp"

void apply_op(QuantumOperator &op, StateVector<data_t> &state){
    bool matched = false; 
    switch (OPMAP[op.name()]){
            //Named gate
        case Opname::x:
            state.apply_x(op.positions()[0]);
            break;
        case Opname::y:
            state.apply_y(op.positions()[0]);
            break;
        case Opname::z:
            state.apply_z(op.positions()[0]);
            break;
        case Opname::h:
            state.apply_h(op.positions()[0]);
            break;
        case Opname::s:
            state.apply_s(op.positions()[0]);
            break;
        case Opname::sdg:
            state.apply_sdag(op.positions()[0]);
            break;
        case Opname::t:
            state.apply_t(op.positions()[0]);
            break;
        case Opname::tdg:
            state.apply_tdag(op.positions()[0]);
            break;
        case Opname::p:
            state.apply_p(op.positions()[0], op.paras()[0]);
            break;
        case Opname::rx:
            state.apply_rx(op.positions()[0], op.paras()[0]);
            break;
        case Opname::ry:
            state.apply_ry(op.positions()[0], op.paras()[0]);
            break;
        case Opname::rz:
            state.apply_rz(op.positions()[0], op.paras()[0]);
            break;
        case Opname::cx:
            state.apply_cnot(op.positions()[0], op.positions()[1]);
            break;
        case Opname::cnot:
            state.apply_cnot(op.positions()[0], op.positions()[1]);
            break;
        case Opname::cp:
            state.apply_cp(op.positions()[0], op.positions()[1], op.paras()[0]);
            break;
        case Opname::cz:
            state.apply_cz(op.positions()[0], op.positions()[1]);
            break;
        case Opname::ccx:
            state.apply_ccx(op.positions()[0], op.positions()[1],  op.positions()[2]);
            break;
        case Opname::toffoli:
            state.apply_ccx(op.positions()[0], op.positions()[1],  op.positions()[2]);
        case Opname::rzz:
            state.apply_cnot(op.positions()[0], op.positions()[1]);
            state.apply_rz(op.positions()[1], op.paras()[0]);
            state.apply_cnot(op.positions()[0], op.positions()[1]);
            break;
        case Opname::measure:
            state.apply_measure(op.qbits(), op.cbits());
            break;
        case Opname::reset:
            state.apply_reset(op.qbits());
            break;
        case Opname::cif:
            // check cbits and condition
            matched = state.check_cif(op.cbits(), op.condition());
            // apply op in instructions
            if(matched){
                for(auto op_h :op.instructions()){
                    apply_op(op_h, state);
                }
            }
            break;
        //Other general gate
        default:
        {
            if (op.targe_num() == 1){
                auto mat_temp = op.mat();
                complex<double> *mat = mat_temp.data();
                if (op.control_num() == 0){
                    state.apply_one_targe_gate_general<0>(op.positions(), mat);
                }else if (op.control_num() == 1){
                    state.apply_one_targe_gate_general<1>(op.positions(), mat);
                }else{
                    state.apply_one_targe_gate_general<2>(op.positions(), mat);
                }
            }else if(op.targe_num() > 1){
                state.apply_multi_targe_gate_general(op.positions(), op.control_num(), op.mat());
            }else{
                throw "Invalid target number";
            }
        }
    }
}

void simulate(Circuit const& circuit, StateVector<data_t> & state){
    state.set_num(circuit.qubit_num());
    state.set_creg(circuit.cbit_num());
    // skip measure and handle it in qfvm.cpp 
    bool skip_measure = circuit.final_measure();
    for (auto op : circuit.instructions()){
        if(skip_measure == true && op.name() == "measure") continue;
        apply_op(op , state);
    }
}

