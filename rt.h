#pragma once


class Value {
public:
        enum Type {
            Scalar,
            Hash,
            Array,
        };

        Value(Type ty) : type(ty), refCount_(1) {
        }

        void addRef() { 
            ++ refCount_; 
        }

        void release() { 
            -- refCount_; 
            if (refCount_ == 0) { 
                delete this; 
            } 
        };

        virtual ~Value() {}

        Type type_;
        int refCount_;
};

class ScalarValue : public Value {
public:
    enum ScalarType {
        Integer,
        Float,
        String,
        Ref.
    };

    ScalarValue(int64_t val) : Value(Scalar), stype_(Integer)  {
       val_.n_val_ = val;
    };

    ScalarValue(double_t val) : Value(Scalar), stype_(Float)  {
       val_.f_val_ = val;
    };

    ScalarValue(const std::string &val) : Value(Scalar), stype_(String)  {
       val_.s_val_ = val;
    };

    ScalarValue(Value *val) : Value(Scalar), stype_(Ref)  {
       val_.r_val_ = val;
    };

    ~ScalarValue() {
        if (type_ == Ref) {
            val_.r_val_->release();
        }
    }

    Scalar_type stype_;

    union {
       int64  n_val_;
       double f_val_;
       Value *r_val_;
    } val_;

    
    std::string s_val_;
};

class ArrayValue : public Value {
public:
    ArrayValue() : Value(Array) {
    }
};

class HashValue : public Value {
public:
    HashValue() : Value(Hash) {
    }
};


struct Frame {
    Frame() : rvalue_(nullptr) {
    }

    Value *rvalue_;
};

