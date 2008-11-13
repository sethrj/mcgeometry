
#ifndef MATERIAL_H
#define MATERIAL_H

#include<iostream>
#include<sstream>

class Material{
    public:
        // Constructor
        Material(double xS, double xF, double xG, double nu);

        // Copy constructor
        Material(const Material& M);

        ~Material() {   };

        double xS() const { return _xS; }
        double xF() const { return _xF; }
        double xG() const { return _xG; }
        double xA() const { return _xA; }
        double xT() const { return _xT; }
        double nu() const { return _nu; }

        friend std::ostream& operator<<(std::ostream& os, const Material& M);

    private:
        double _xS; //Scattering cross section
        double _xF; //Fission cross section
        double _xG; //Capture cross section
        double _nu; //Average number of fission neutrons per fission
        double _xA; //Absorption cross section
        double _xT; //Total cross section
};

Material::Material(double xS, double xF, double xG, double nu)
    : _xS(xS), _xF(xF), _xG(xG), _nu(nu),
      _xA(_xF + _xG), _xT(_xA + _xF) {
}

Material::Material(const Material& M)
    : _xS(M.xS()), _xF(M.xF()), _xG(M.xG()), _nu(M.nu()),
      _xA(_xF + _xG), _xT(_xA + _xF) {
}

inline std::ostream& operator<<(std::ostream& os, const Material& M){
    os << "[(nu: " << M._nu << ", xF: "  << M._xF << ", xG: " << M._xG
      <<", xS: " << M._xS << ") , xA: " << M._xA << ", xT: " << M._xT << "]";
    return os;
}
#endif

