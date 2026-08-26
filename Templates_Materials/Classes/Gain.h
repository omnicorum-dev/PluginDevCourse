/*
  ==============================================================================

    Gain.h
    Created: 11 Mar 2026 4:59:15pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Basics.h"
#include "MonoEffect.h"

class Gain : public MonoEffect {
public:
    
    void setGainLinear (float _gain_linear) {
        gain_linear = _gain_linear;
    }
    
    void setGainDB (float _gain_db) {
        gain_linear = basics::db2mag(_gain_db);
    }
    
    float processSample (float xn) override {
        return xn * gain_linear;
    }
    
protected:
    float gain_linear = 1.0f;
};
