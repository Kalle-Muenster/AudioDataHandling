#ifndef DefineAudioFrameTypeStructure
#define DefineAudioFrameTypeStructure( sigT, tySi, chCn ) \
[StructLayoutAttribute(LayoutKind::Explicit,Size=(tySi>>3)*chCn)] \
public value class AuPCM##sigT##tySi##bit##chCn##ch : public Stepflow::Audio::IAudioFrame { \
public: \
	typedef stepflow::Frame<stepflow::sigT##tySi,chCn> NATIVE; \
	typedef AuPCM##sigT##tySi##bit##chCn##ch TYPE; \
    typedef stepflow::##sigT##tySi NTYP; \
    typedef Stepflow::##sigT##tySi STYP; \
	const static Stepflow::Audio::AudioFrameType type = Stepflow::Audio::AudioFrameType(NATIVE::TS,chCn); \
    [FieldOffsetAttribute(0)] STYP sampledata; \
    property Stepflow::Audio::AudioFrameType FrameType { \
        virtual Stepflow::Audio::AudioFrameType get(void) { \
            return TYPE::type; \
        } \
    } \
	virtual IntPtr GetRaw(void) { \
        pin_ptr<STYP> pt = &sampledata; \
		return IntPtr( (void*)pt ); \
	} \
	TYPE Mix(STYP sample, Stepflow::Audio::Panorama mixer) { \
		getNative()->mix( NTYP(sample), *mixer ); \
		return *this; \
	} \
	virtual Stepflow::Audio::IAudioFrame^ Mix(Object^ sample, Stepflow::Audio::Panorama mixer) { \
		getNative()->mix( NTYP(SampleTypeConversion<NATIVE,STYP>( sample )), *mixer ); \
		return safe_cast<IAudioFrame^>(*this); \
	} \
	virtual Stepflow::Audio::IAudioFrame^ Mix(Stepflow::Audio::IAudioFrame^ frame, float mix) { \
		getNative()->mix( *safe_cast<TYPE>(frame).getNative(), mix ); \
		return safe_cast<Stepflow::Audio::IAudioFrame^>(*this); \
	} \
	virtual Stepflow::Audio::IAudioFrame^ Amp(float amplify) { \
		getNative()->amp( amplify ); \
		return safe_cast<Stepflow::Audio::IAudioFrame^>(*this); \
	} \
	virtual Stepflow::Audio::IAudioFrame^ Pan(Stepflow::Audio::Panorama p) { \
		getNative()->pan( *p , stepflow::Panorama::Axis::Any ); \
		return safe_cast<Stepflow::Audio::IAudioFrame^>(*this); \
	} \
	virtual Stepflow::Audio::IAudioFrame^ Pan(float mixer, Stepflow::Audio::Panorama::Axis axis) { \
		stepflow::Panorama::Axis ax = stepflow::Panorama::Axis((int)axis); \
        stepflow::Panorama p; p.set(ax, mixer); \
		getNative()->pan( p, ax ); \
		return safe_cast<Stepflow::Audio::IAudioFrame^>(*this); \
	} \
	virtual Stepflow::Audio::IAudioFrame^ Pan( Panorama pan, Stepflow::Audio::Panorama::Axis axe ) { \
		getNative()->pan( *pan, stepflow::Panorama::Axis(axe) ); \
		return safe_cast<Stepflow::Audio::IAudioFrame^>(*this); \
	} \
	virtual Stepflow::Audio::IAudioFrame^ Pan(float mixer,char axis) { \
		stepflow::Panorama::Axis ax = stepflow::Panorama::Axis(axis); \
        stepflow::Panorama p; p.set(ax, mixer); \
		getNative()->pan( p, ax ); \
		return safe_cast<Stepflow::Audio::IAudioFrame^>(*this); \
	} \
    TYPE( double initValue ) { \
        sampledata = (STYP)initValue; \
        for(int i=1;i<chCn;i++) \
            *((&sampledata)+i) = sampledata; \
    } \
    TYPE( IntPtr pointerToFrame ) { \
        setNative( (NATIVE*)pointerToFrame.ToPointer() ); \
    } \
	virtual void SetChannel(int chIdx, STYP value) { \
		*((&sampledata)+chIdx) = value; \
	} \
	virtual void SetChannel(int chIdx, IntPtr ptValue) { \
		*((&sampledata)+chIdx) = *(STYP*)ptValue.ToPointer(); \
	} \
	virtual STYP GetChannel(int idx) { \
		return *((&sampledata)+idx); \
	} \
    virtual IntPtr GetPointer(int channel) { \
        pin_ptr<STYP> ptr = (&sampledata)+channel; \
		return IntPtr(ptr); \
	} \
	virtual property STYP default[int] { \
		STYP get(int idx) { return *((&sampledata)+idx); } \
		void set(int idx,STYP value) { *((&sampledata)+idx) = value; } \
	} \
	virtual property Object^ Channel[int] { \
		Object^ get(int idx) { return *((&sampledata)+idx); } \
		void set(int idx,Object^ value) { \
            STYP sample = safe_cast<STYP>(value); \
            *((&sampledata)+idx) = sample; \
        } \
	} \
    virtual Stepflow::Audio::IAudioFrame^ Convert( AudioFrameType toFrameType ) { \
        if ( FrameType.Code != toFrameType.Code ) { \
            Stepflow::Audio::IAudioFrame^ convFrm = toFrameType.CreateEmptyFrame(); \
            return this->Mix( convFrm, 1.0f ); \
        } return *this; \
    } \
    static TYPE operator * ( TYPE This, TYPE That ) { \
        for( int c=0; c<chCn; ++c ) { \
            That[c] *= This[c]; \
        } return That; \
    } \
    static TYPE operator + ( TYPE This, TYPE That ) { \
        for( int c=0; c<chCn; ++c ) { \
            That[c] += This[c]; \
        } return That; \
    } \
    static TYPE operator - ( TYPE This, TYPE That ) { \
        for( int c=0; c<chCn; ++c ) { \
            This[c] -= That[c]; \
        } return This; \
    } \
    TYPE operator - () { \
        TYPE invert; \
        for( int c=0; c<chCn; ++c ) { \
            invert[c] = STYP( stepflow::datatype_limits<NTYP>::db0() - ( (NTYP)(*this)[c] - stepflow::datatype_limits<NTYP>::db0() )); \
        } return invert; \
    } \
    virtual Stepflow::Audio::IAudioFrame^ multiplicationResult( Stepflow::Audio::IAudioFrame^ factors) { \
        return safe_cast<Stepflow::Audio::IAudioFrame^>( *this * safe_cast<TYPE>(factors) ); \
    } \
    virtual Stepflow::Audio::IAudioFrame^ additionResult(Stepflow::Audio::IAudioFrame^ additive) { \
        return safe_cast<Stepflow::Audio::IAudioFrame^>( *this + safe_cast<TYPE>(additive) ); \
    } \
    virtual Stepflow::Audio::IAudioFrame^ subtractionResult(Stepflow::Audio::IAudioFrame^ extractive) { \
        return safe_cast<Stepflow::Audio::IAudioFrame^>( *this - safe_cast<TYPE>(extractive) ); \
    } \
    TYPE operator *= ( TYPE That ) { \
        for( int c=0; c<chCn; ++c ) { \
             (*this)[c] *= That[c]; \
        } return *this; \
    } \
    TYPE operator += ( TYPE That ) { \
        for( int c=0; c<chCn; ++c ) { \
            (*this)[c] += That[c]; \
        } return *this; \
    } \
    TYPE operator -= ( TYPE That ) { \
        for( int c=0; c<chCn; ++c ) { \
            (*this)[c] -= That[c]; \
        } return *this; \
    } \
    virtual Stepflow::Audio::IAudioFrame^ Multiply( Stepflow::Audio::IAudioFrame^ factors) { \
        return  safe_cast<Stepflow::Audio::IAudioFrame^>( *this *= safe_cast<TYPE>(factors) ); \
    } \
    virtual Stepflow::Audio::IAudioFrame^ Add( Stepflow::Audio::IAudioFrame^ additive ) { \
        return  safe_cast<Stepflow::Audio::IAudioFrame^>( *this += safe_cast<TYPE>(additive) ); \
    } \
    virtual Stepflow::Audio::IAudioFrame^ Subtract( Stepflow::Audio::IAudioFrame^ extractive ) { \
        return  safe_cast<Stepflow::Audio::IAudioFrame^>( *this -= safe_cast<TYPE>(extractive) ); \
    } \
    virtual Stepflow::Audio::IAudioFrame^ inverse( void ) { \
        return  safe_cast<Stepflow::Audio::IAudioFrame^>( -(*this) ); \
    } \
    virtual Panorama GetPanorama( void ) { \
        return Panorama( getNative()->panorama() ); \
    } \
    System::String^ ToString(void) override { \
        System::Text::StringBuilder^ stringer = \
            gcnew System::Text::StringBuilder(FrameType.ToString()); \
        stringer->Append(": ["); \
        for( int i=0; i<NATIVE::CH; ++i ) { \
            stringer->Append(System::String::Format( "{0}", GetChannel(i).ToString() )); \
            if( i < (NATIVE::CH - 1) ) stringer->Append(","); \
        } stringer->Append( "]" ); \
        return stringer->ToString(); \
    } \
    virtual void Clear(void) { \
        sampledata = 0; \
        for(int i=1;i<chCn;i++) \
            *((&sampledata)+i) = stepflow::datatype_limits<NTYP>::db0(); \
    } \
    virtual Stepflow::Audio::IAudioFrame^ Clone( void ) override { \
        TYPE clone = safe_cast<TYPE>( FrameType.CreateEmptyFrame() ); \
        return clone.setNative( this->getNative() ); \
    } \
    virtual Stepflow::Audio::IAudioFrame^ Set( Stepflow::Audio::IAudioFrame^ assginee ) { \
        return safe_cast<Stepflow::Audio::IAudioFrame^>( *this = safe_cast<TYPE>( assginee ) ); \
    } \
    virtual void SetRaw( System::IntPtr data ) { \
        *this = *(TYPE*)data.ToPointer(); \
    } \
internal: \
	NATIVE* getNative(void) { \
        pin_ptr<STYP> p = &sampledata; \
        NTYP* nPt = (NTYP*)p; \
		return (NATIVE*)nPt; \
	} \
	Stepflow::Audio::IAudioFrame^ setNative( NATIVE* val ) { \
		for( int i=0; i<chCn; i++ ) \
			(&sampledata)[i] = STYP( NTYP( val->channel[i] ) ); \
        return safe_cast<Stepflow::Audio::IAudioFrame^>(*this); \
	} \
}

#define MANAGEDFRAME_TYPECODE( typi ) ( typi ## ::NATIVE::TS + (( typi ## ::NATIVE::CH | (typi ## ::NATIVE::ST==1?3:typi##::NATIVE::ST+1)<<6)<<8)  )
#define MANAGEDFRAME_TYPE( eenz, zwee, dree ) Stepflow::Audio::FrameTypes::Au ## eenz ## zwee ## bit ## dree ## ch
#define ManagedFrameCase( NumberOne, NumberTwo, NumberTri, actionOne ) case \
        AUDIOFRAME_CODE( NumberOne, NumberTwo, NumberTri ): { \
            typedef MANAGEDFRAME_TYPE( NumberTri, NumberOne, NumberTwo ) CASE_TYPE; \
            actionOne \
        } break;


#define MANAGEDTYPE_SWITCH( tyco, actionToDo ) \
    switch (tyco) { \
        ManagedFrameCase(8,1, PCMs,actionToDo ) \
        ManagedFrameCase(8,2, PCMs,actionToDo ) \
        ManagedFrameCase(8,4, PCMs,actionToDo ) \
        ManagedFrameCase(8,6, PCMs,actionToDo ) \
        ManagedFrameCase(8,8, PCMs,actionToDo ) \
        ManagedFrameCase(16,1,PCMs,actionToDo ) \
        ManagedFrameCase(16,2,PCMs,actionToDo ) \
        ManagedFrameCase(16,4,PCMs,actionToDo ) \
        ManagedFrameCase(16,6,PCMs,actionToDo ) \
        ManagedFrameCase(16,8,PCMs,actionToDo ) \
        ManagedFrameCase(24,1,PCMs,actionToDo ) \
        ManagedFrameCase(24,2,PCMs,actionToDo ) \
        ManagedFrameCase(24,4,PCMs,actionToDo ) \
        ManagedFrameCase(24,6,PCMs,actionToDo ) \
        ManagedFrameCase(24,8,PCMs,actionToDo ) \
        ManagedFrameCase(32,1,PCMf,actionToDo ) \
        ManagedFrameCase(32,2,PCMf,actionToDo ) \
        ManagedFrameCase(32,4,PCMf,actionToDo ) \
        ManagedFrameCase(32,6,PCMf,actionToDo ) \
        ManagedFrameCase(32,8,PCMf,actionToDo ) \
        ManagedFrameCase(64,1,PCMf,actionToDo ) \
        ManagedFrameCase(64,2,PCMf,actionToDo ) \
        ManagedFrameCase(64,4,PCMf,actionToDo ) \
        ManagedFrameCase(64,6,PCMf,actionToDo ) \
        ManagedFrameCase(64,8,PCMf,actionToDo ) \
        default: { \
    throw gcnew System::Exception(UNKNOWN_TYPECODE); \
                     } break; \
}

#endif
