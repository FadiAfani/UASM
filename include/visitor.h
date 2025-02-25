#ifndef VISIOR_H
#define VISIOR_H

class Visitor {
    public:
        virtual ~Visitor();
        virtual void visit(Visitor& visitor) = 0;
};



#endif
