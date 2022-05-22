/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      Panorama.hpp                               ||
||     Author:    Kalle                                      ||
||     Generated: 28.07.2018                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _Panorama_hpp_
#define _Panorama_hpp_


BEGIN_WAVESPACE

struct WAVELIB_API Panorama;
WAVELIB_API std::string PictogramFromPan(Panorama pan);

struct WAVELIB_API Panorama
{
    enum CONSTANTS : unsigned long long {
        STEREO_MASK = 0x00000000ffffffff,
        FRONTAL_MASK = 0xffffffff00000000,
        INVALID = 0xFFC00000FFC00000,
        NONFRONTAL = 0x3F00000000000000,
        CENTERED = 0x000000003F000000,
        NEUTRAL = 0x3F0000003F000000, // NONFRONTAL|CENTERED
        LEFT =  0x000000003f800000,  
        RIGHT = 0x0000000000000000,
        FRONT = 0x3f80000000000000,
        REAR = 0x0000000000000000
    };
    union PanData {
        unsigned long long I64;
        float              F32[2];
    } data;

    enum Axis : char {
        Any = 0,
        LeftToRight = 1,
        FrontToRear = 2
    };

    enum Modus : char {
        Absolute,
        Subractive,
        Modi
    };
    static float ampFactor( float amplifier, Modus mode ) {
        return mode ? amplifier > 1.f ? 1.f : amplifier : amplifier;
    }
    static Panorama extract_stereo(Panorama surround) {
        return CONSTANTS(NONFRONTAL | (STEREO_MASK&surround.data.I64));
    }
    static Panorama extract_frontal(Panorama surround) {
        return CONSTANTS(CENTERED | (FRONTAL_MASK&surround.data.I64));
    }
    static Panorama movingStep( Panorama from, Panorama ends, uint steps ) {
        return Panorama( (float)(((double)ends.get(LeftToRight) - (double)from.get(LeftToRight)) / steps),
                         (float)(((double)ends.get(FrontToRear) - (double)from.get(FrontToRear)) / steps) );
    }
    static Panorama morphPoint( Panorama from, Panorama ends, double propoposition ) {
        double lr = from.get(LeftToRight); double fr = from.get(FrontToRear);
        return Panorama( (float)(lr + (((double)ends.get(LeftToRight) - lr) * propoposition)),
                         (float)(fr + (((double)ends.get(FrontToRear) - fr) * propoposition)));
    }
    void addStep(Panorama step) {
        set(LeftToRight, this->get(LeftToRight) + step.get(LeftToRight));
        set(FrontToRear, this->get(FrontToRear) + step.get(FrontToRear));
    }
    float get(Axis axis) const {
        return data.F32[axis-1];
    }
    void  set(Axis axis, float mixe) {
        data.F32[axis-1] = mixe;
    }

    float get_Stereo(void) const {
        return get(LeftToRight);
    }
    void  set_Stereo(float val) {
        set(LeftToRight, val);
    }
    float get_Frontal(void) const {
        return get(FrontToRear);
    }
    void set_Frontal(float val) {
        set(FrontToRear, val);
    }

    std::string getPictogram() {
        return PictogramFromPan( *this );
    }

    float get_Left(Modus m) { return ampFactor( get(LeftToRight)*2.f, m ); }
    float get_Right(Modus m) { return ampFactor( (1.0f-get(LeftToRight))*2.f, m ); }
    float get_Front(Modus m) { return ampFactor( get(FrontToRear)*2.f, m ); }
    float get_Rear(Modus m) { return ampFactor( (1.0f-get(FrontToRear))*2.f, m ); }

    void set_Left(Modus m, float value) { set(LeftToRight, ampFactor( (value/get_Right(m))*0.5f, m)); }
    void set_Right(Modus m, float value) { set(LeftToRight, ampFactor( (get_Left(m)/value)*0.5f, m)); }
    void set_Front(Modus m, float value) { set(FrontToRear, ampFactor( (value/get_Rear(m))*0.5f, m)); }
    void set_Rear(Modus m, float value) { set(FrontToRear, ampFactor( (get_Front(m)/value)*0.5f, m)); }

#ifdef _MSC_VER
    DECLPROP(float,Stereo);
    DECLPROP(float,Frontal);

    LISTPROP(float,Left,Modi);
    LISTPROP(float,Right,Modi);
    LISTPROP(float,Front,Modi);
    LISTPROP(float,Rear,Modi);
#endif

    Panorama(void)
        : data(PanData{NEUTRAL}) {
    }
    Panorama(ulong raw)
        : data(PanData{raw}) {

    }
    Panorama(int one)
        : Panorama( (one<0 ? 0.0f : one>0 ? 1.0f : 0.5f) ) {
    }
    Panorama(const Panorama& copy)
        : data(copy.data) {
    }
    Panorama(float LeftRight, float FrontRear) {
        set(LeftToRight, LeftRight);
        set(FrontToRear, FrontRear);
    }
    Panorama(CONSTANTS value)
        : data(PanData{value}) {
    }
    Panorama(float value, Axis axis = Any) {
        if (axis == Any) {
            set(LeftToRight, value);
            set(FrontToRear, value);
        } else {
            set(Axis(3-axis), 0.5f);
            set(axis, value);
        }
    }

    Panorama operator =(CONSTANTS other) {
        data.I64 = other ? other : NEUTRAL;
        return *this;
    }
    operator float*() {
        return &data.F32[0];
    }

    Panorama operator +(Panorama other) const {
        return Panorama( (( (get(LeftToRight)*2.0-1.0) + (other.get(LeftToRight)*2.0-1.0)) / 2.0f ),
                         (( (get(FrontToRear)*2.0-1.0) + (other.get(FrontToRear)*2.0-1.0)) / 2.0f ) );
    }
    Panorama operator -(Panorama other) const {
        return Panorama( (((get(LeftToRight)*2.0f-1.0f) - (other.get(LeftToRight)*2.0f-1.0f))+1.0f)/2.0f,
                         (((get(FrontToRear)*2.0f-1.0f) - (other.get(FrontToRear)*2.0f-1.0f))+1.0f)/2.0f );
    }
    Panorama operator *(Panorama other) const {
        return Panorama( (get(LeftToRight) * (other.get(LeftToRight) * 2.0f)),
                         (get(FrontToRear) * (other.get(FrontToRear) * 2.0f)) );
    }
    Panorama operator /(Panorama other) const {
        return Panorama( (get(LeftToRight) * ((1.0f - other.get(LeftToRight)) * 2.0f)),
                         (get(FrontToRear) * ((1.0f - other.get(FrontToRear)) * 2.0f)) );
    }

    Panorama operator *(float scalar) const {
        return Panorama( (get(LeftToRight) * scalar),
                         (get(FrontToRear) * scalar) );
    }
    Panorama operator /(float scalar) const {
        return Panorama( (get(LeftToRight) / scalar),
                         (get(FrontToRear) / scalar) );
    }

    bool operator ==(const Panorama& other) const {
        return (get(LeftToRight) == other.get(LeftToRight))
            && (get(FrontToRear) == other.get(FrontToRear));
    }
    bool operator !=(const Panorama& other) const {
        return !this->operator==(other);
    }
    bool operator <(const Panorama& other) const {
        return get(LeftToRight) + get(FrontToRear) < other.get(LeftToRight) + other.get(FrontToRear);
    }
    bool operator >(const Panorama& other) const {
        return get(LeftToRight) + get(FrontToRear) > other.get(LeftToRight) + other.get(FrontToRear);
    }
    bool operator <=(const Panorama& other) const {
        return get(LeftToRight) + get(FrontToRear) <= other.get(LeftToRight) + other.get(FrontToRear);
    }
    bool operator >=(const Panorama& other) const {
        return get(LeftToRight) + get(FrontToRear) >= other.get(LeftToRight) + other.get(FrontToRear);
    }

    Panorama operator -(void) const {
        return Panorama( 1.0f - get(LeftToRight),
                         1.0f - get(FrontToRear) );
    }

    bool isEffective(void) const {
        return data.I64 != NEUTRAL;
    }
    bool effectsStereo(void) const {
        return (isEffective() && (data.I64 & STEREO_MASK));
    }
    bool effectsFrontal(void) const {
        return (isEffective() && (data.I64 & FRONTAL_MASK));
    }
    Panorama& operator +=(const Panorama& other) {
        set(LeftToRight, (get(LeftToRight) + other.get(LeftToRight)) / 2.0f);
        set(FrontToRear, (get(FrontToRear) + other.get(FrontToRear)) / 2.0f);
        return *this;
    }
    Panorama& operator -=(const Panorama& other) {
        set(LeftToRight, get(LeftToRight) + (other.get(LeftToRight) - get(LeftToRight)) / 2.0f);
        set(FrontToRear, get(FrontToRear) + (other.get(FrontToRear) - get(FrontToRear)) / 2.0f);
        return *this;
    }
    Panorama& operator *=(const Panorama& other) {
        set(LeftToRight, (get(LeftToRight) * (other.get(LeftToRight) * 2.0f)));
        set(FrontToRear, (get(FrontToRear) * (other.get(FrontToRear) * 2.0f)));
        return *this;
    }
    Panorama& operator /=(const Panorama& other) {
        set(LeftToRight, (get(LeftToRight) * ((1.0f - other.get(LeftToRight)) * 2.0f)));
        set(FrontToRear, (get(FrontToRear) * ((1.0f - other.get(FrontToRear)) * 2.0f)));
        return *this;
    }
    Panorama cast_stereo(void) const {
        return Panorama::extract_stereo(*this);
    }
    Panorama cast_frontal(void) const {
        return Panorama::extract_frontal(*this);
    }
    Panorama flipped(Axis axis = Any) const {
        switch (axis) {
        case Any:         return Panorama( get(FrontToRear), get(LeftToRight) );
        case LeftToRight: return Panorama(1.f-get(LeftToRight),get(FrontToRear));
        case FrontToRear: return Panorama(get(LeftToRight),1.f-get(FrontToRear));
        default: return CONSTANTS::INVALID;
        }
    }

};

ENDOF_WAVESPACE
#endif
