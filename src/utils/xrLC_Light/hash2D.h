#pragma once

template <class T, u32 s_X, u32 s_Y>
class hash2D 
{
    xr_vector<T>    table[s_Y][s_X];
    Fbox2            bounds;
    Fvector2        size;
public:
    hash2D()
    {
        bounds.invalidate    ();
        size.set            (0.f,0.f);
    }

    ~hash2D()
    {
    }

    void        initialize    (Fbox2& R, u32 faces)
    {
        bounds.set    (R);
        size.set    (R.max.x-R.min.x,R.max.y-R.min.y);

        u32        size    = s_Y*s_X;
        u32        apx        = faces/size;

        for (u32 y=0; y<s_Y; y++)
            for (u32 x=0; x<s_X; x++)
            {
                table[y][x].clear();
                table[y][x].reserve            (apx);
            }
    };
    void        add            (Fbox2& R, T& value)
    {
        int x1 = iFloor(float(s_X)*(R.min.x-bounds.min.x)/size.x); clamp(x1,0,int(s_X-1));
        int x2 = iCeil (float(s_X)*(R.max.x-bounds.min.x)/size.x); clamp(x2,0,int(s_X-1));
        int y1 = iFloor(float(s_Y)*(R.min.y-bounds.min.y)/size.y); clamp(y1,0,int(s_Y-1));
        int y2 = iCeil (float(s_Y)*(R.max.y-bounds.min.y)/size.y); clamp(y2,0,int(s_Y-1));
        
        for (int y=y1; y<=y2; y++)
            for (int x=x1; x<=x2; x++)
                table[y][x].push_back(value);
    };
    xr_vector<T>&    query        (float x, float y)
    {
        int _x    = iFloor(float(s_X)*(x-bounds.min.x)/size.x); clamp(_x,0,int(s_X-1));
        int _y    = iFloor(float(s_Y)*(y-bounds.min.y)/size.y); clamp(_y,0,int(s_Y-1));
        return table[_y][_x];
    };

    template<typename TP>    
    struct get_type
    {
        typedef    TP    type    ;
    };
    
    template<typename TP>    
    struct get_type<TP*>
    {
        typedef    TP    type    ;
    };

    typedef     typename get_type<T>::type        type;
 
};
