#ifndef VIEW_H
#define VIEW_H

#include <VrMath/LinearR3.h>

class View
{
public:
    View()
    {
        Reset();
    }

    void Reset();

    void SetLocalViewer()
    {
        LocalViewer = true;
    }
    void SetNonLocalViewer()
    {
        LocalViewer = false;
    }
    bool IsLocalViewer() const
    {
        return LocalViewer;
    }

    virtual void SetPosition( double x, double y, double z );
    virtual void SetPosition( const double* );
    virtual void SetPosition( const float* );
    virtual void SetPosition( const VectorR3& pos );
    void GetPosition( double* ) const;
    void GetPosition( float* ) const;
    const VectorR3& GetPosition() const
    {
        return Position;
    }

    virtual void SetDirection( double x, double y, double z );
    virtual void SetDirection( const double* );
    virtual void SetDirection( const float* );
    virtual void SetDirection( const VectorR3& dir );
    void GetDirection( double* ) const;
    void GetDirection( float* ) const;
    const VectorR3& GetDirection() const
    {
        return Direction;
    }

protected:
    VectorR3 Position;
    VectorR3 Direction;	// Viewer direction (unit vector).
    bool LocalViewer;	// True if is a local viewer
    // If local viewer, only Position is used
    // If non-local viewer, only the Direction is used.
};

inline void View::Reset()
{
    Position.SetZero();
    Direction.SetNegUnitZ();
    SetLocalViewer();
}

inline void View::SetPosition( double x, double y, double z )
{
    Position.Set(x, y, z);
}

inline void View::SetPosition( const double* pos)
{
    SetPosition( *pos, *(pos+1), *(pos+2) );
}

inline void View::SetPosition( const float* pos )
{
    SetPosition( *pos, *(pos+1), *(pos+2) );
}

inline void View::SetPosition( const VectorR3& pos )
{
    SetPosition( pos.x, pos.y, pos.z );
}

inline void View::GetPosition( double* pos ) const
{
    *pos = Position.x;
    *(pos+1) = Position.y;
    *(pos+2) = Position.z;
}

inline void View::GetPosition( float* pos ) const
{
    *pos = (float)Position.x;
    *(pos+1) = (float)Position.y;
    *(pos+2) = (float)Position.z;
}

inline void View::SetDirection( double x, double y, double z )
{
    Direction.Set(x, y, z);
    Direction.Normalize();
}

inline void View::SetDirection( const double* dir)
{
    SetDirection( *dir, *(dir+1), *(dir+2) );
}

inline void View::SetDirection( const float* dir )
{
    SetDirection( *dir, *(dir+1), *(dir+2) );
}

inline void View::SetDirection( const VectorR3& dir )
{
    SetDirection( dir.x, dir.y, dir.z );
}

inline void View::GetDirection( double* dir ) const
{
    *dir = Direction.x;
    *(dir+1) = Direction.y;
    *(dir+2) = Direction.z;
}

inline void View::GetDirection( float* dir ) const
{
    *dir = (float)Direction.x;
    *(dir+1) = (float)Direction.y;
    *(dir+2) = (float)Direction.z;
}
#endif // VIEW_H
