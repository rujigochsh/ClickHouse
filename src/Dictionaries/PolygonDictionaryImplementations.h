#pragma once

#include "PolygonDictionary.h"
#include "PolygonDictionaryUtils.h"

#include <vector>

namespace DB
{

/** Simple implementation of the polygon dictionary. Doesn't generate anything during its construction.
  * Iterates over all stored polygons for each query, checking each of them in linear time.
  * Retrieves the polygon with the smallest area containing the given point. 
  * If there is more than one any such polygon may be returned.
  */
class PolygonDictionarySimple : public IPolygonDictionary
{
public:
    PolygonDictionarySimple(
            const std::string & database_,
            const std::string & name_,
            const DictionaryStructure & dict_struct_,
            DictionarySourcePtr source_ptr_,
            DictionaryLifetime dict_lifetime_,
            InputType input_type_,
            PointType point_type_);

    std::shared_ptr<const IExternalLoadable> clone() const override;

private:
    bool find(const Point & point, size_t & id) const override;
};

/** A polygon dictionary which generates a recursive grid in order to efficiently cut the number
  * of polygons to be checked for a given point. 
  * For more detail see the GridRoot and FinalCell classes.
  * Separately, a slab index is built for each individual polygon. This allows to check the 
  * candidates more efficiently. 
  */
class PolygonDictionaryIndexEach : public IPolygonDictionary
{
public:
    PolygonDictionaryIndexEach(
            const std::string & database_,
            const std::string & name_,
            const DictionaryStructure & dict_struct_,
            DictionarySourcePtr source_ptr_,
            DictionaryLifetime dict_lifetime_,
            InputType input_type_,
            PointType point_type_,
            int min_intersections_,
            int max_depth_);

    std::shared_ptr<const IExternalLoadable> clone() const override;

    static constexpr size_t kMinIntersectionsDefault = 1;
    static constexpr size_t kMaxDepthDefault = 5;

private:
    bool find(const Point & point, size_t & id) const override;

    std::vector<SlabsPolygonIndex> buckets;
    GridRoot<FinalCell> grid;

    const size_t min_intersections;
    const size_t max_depth;
};

/** Uses single SlabsPolygonIndex for all queries. */
class PolygonDictionaryIndexCell : public IPolygonDictionary
{
public:
    PolygonDictionaryIndexCell(
            const std::string & database_,
            const std::string & name_,
            const DictionaryStructure & dict_struct_,
            DictionarySourcePtr source_ptr_,
            DictionaryLifetime dict_lifetime_,
            InputType input_type_,
            PointType point_type_,
            size_t min_intersections_,
            size_t max_depth_);

    std::shared_ptr<const IExternalLoadable> clone() const override;

    static constexpr size_t kMinIntersectionsDefault = 1;
    static constexpr size_t kMaxDepthDefault = 5;

private:
    bool find(const Point & point, size_t & id) const override;

    GridRoot<FinalCellWithSlabs> index;

    const size_t min_intersections;
    const size_t max_depth;
};

}

