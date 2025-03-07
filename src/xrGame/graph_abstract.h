////////////////////////////////////////////////////////////////////////////
//    Module         : graph_abstract.h
//    Created     : 14.01.2004
//  Modified     : 19.02.2005
//    Author        : Dmitriy Iassenev
//    Description : Graph class template
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_vertex.h"
#include "graph_edge.h"
#include "../xrServerEntities/object_broker.h"

template <
    typename _data_type = Loki::EmptyType,
    typename _edge_weight_type = float,
    typename _vertex_id_type = u32,
    typename _edge_data_type = Loki::EmptyType
>
class CGraphAbstract {
public:
    typedef CVertex<
        _data_type,
        _vertex_id_type,
        CGraphAbstract
    >                                                CVertex;

    typedef CEdge<
        _edge_weight_type,
        CVertex,
        _edge_data_type
    >                                                CEdge;

public:
    typedef xr_map<_vertex_id_type,CVertex*>        VERTICES;
    typedef typename CVertex::EDGES                    EDGES;

public:
    typedef typename VERTICES::const_iterator        const_vertex_iterator;
    typedef typename VERTICES::iterator                vertex_iterator;
    typedef typename EDGES::const_iterator            const_iterator;
    typedef typename EDGES::iterator                iterator;
    typedef _vertex_id_type                            _vertex_id_type;

private:
    VERTICES                    m_vertices;
    u32                            m_edge_count;

public:
    IC                                    CGraphAbstract    ();
    virtual                                ~CGraphAbstract    ();
    IC        bool                        Search            (_vertex_id_type start_vertex_id, _vertex_id_type dest_vertex_id,xr_vector<_vertex_id_type>& OutPath, _edge_weight_type MaxRange = type_max(_edge_weight_type), u32 MaxIterationCount = 0xFFFFFFFF,u32 MaxVisitedNodeCount = 0xFFFFFFFF, _edge_weight_type* LastCost = nullptr) const;
    IC        bool                        operator==        (const CGraphAbstract &obj) const;
    IC        void                        clear            ();
    IC        void                        add_vertex        (const _data_type &data, const _vertex_id_type &vertex_id);
    IC        void                        remove_vertex    (const _vertex_id_type &vertex_id);
    IC        void                        add_edge        (const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1, const _edge_weight_type &edge_weight);
    IC        void                        add_edge        (const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1, const _edge_weight_type &edge_weight0, const _edge_weight_type &edge_weight1);
    IC        void                        remove_edge        (const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1);
    IC        u32                            vertex_count    () const;
    IC        u32                            edge_count        () const;
    IC        bool                        empty            () const;
    IC        const VERTICES                &vertices        () const;
    IC        VERTICES                    &vertices        ();
    IC        const CVertex                *vertex            (const _vertex_id_type &vertex_id) const;
    IC        CVertex                        *vertex            (const _vertex_id_type &vertex_id);
    IC        const CEdge                    *edge            (const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1) const;
    IC        CEdge                        *edge            (const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1);
    IC        const CGraphAbstract        &header            () const;
    IC        const _edge_weight_type        get_edge_weight    (const _vertex_id_type vertex_index0, const _vertex_id_type vertex_index1, const_iterator i) const;
    IC        bool                        is_accessible    (const _vertex_id_type vertex_index) const;
    IC        _vertex_id_type const        &value            (_vertex_id_type const &vertex_index, const_iterator i) const;
    IC        void                        begin            (const CVertex *vertex, const_iterator &b, const_iterator &e) const;
    IC        void                        begin            (_vertex_id_type const &vertex_index, const_iterator &b, const_iterator &e) const;
};

template <
    typename _data_type = Loki::EmptyType,
    typename _edge_weight_type = float,
    typename _vertex_id_type = u32
>
class CGraphAbstractSerialize : public CGraphAbstract<_data_type,_edge_weight_type,_vertex_id_type>, public IPureSerializeObject<IReader,IWriter> {
public:
    virtual void                        save            (IWriter &stream);
    virtual void                        load            (IReader &stream);
};

#include "graph_abstract_inline.h"