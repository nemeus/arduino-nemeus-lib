/////////////////////////////////////////////////////////////////////////////
//
// Singleton - Template for Singleton applicable on any class (code from internet)
//
/////////////////////////////////////////////////////////////////////////////
#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
class Singleton
{
  protected:
    // Constructeur/destructeur
    Singleton () { }
    ~Singleton () {  }

  public:
    // Interface publique
    static T *getInstance ()
    {
      if (NULL == _singleton)
      {
        _singleton = new T;
      }
      else
      {
        
      }

      return (static_cast<T*> (_singleton));
    }

    static void kill ()
    {
      if (NULL != _singleton)
      {
        delete _singleton;
        _singleton = NULL;
      }
    }

  private:
    // Unique instance
    static T *_singleton;
};

template <typename T>
T *Singleton<T>::_singleton = NULL;

#endif
