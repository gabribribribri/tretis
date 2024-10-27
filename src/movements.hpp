#pragma once

class Movements {
    
    
    
public:
    static Movements& Get() {
        static Movements instance;
        return instance;
    }
    
    Movements(const Movements&) = delete;
    Movements(Movements&&) = delete;
    Movements operator=(Movements) = delete;
private:
    Movements() = default;
    ~Movements() = default;
};

