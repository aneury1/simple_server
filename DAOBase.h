#pragma once

template<typename Class, typename PK, typename SK>
class DaoBase
{
    protected:

       ConnectionHandler *connection_handler;

    public:

    DaoBase(ConnectionHandler *handler): connection_handler{handler}{}

    virtual Class get(PK id) =0;
  
    virtual std::vector<Class> getAll()=0;

    virtual void Insert(Class cl)=0;
    
    virtual void Update(Class cl, PK* id)=0;

    virtual void Delete(Class cl, PK* id)=0;
};
