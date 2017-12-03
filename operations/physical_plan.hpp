/*
执行顺序


语法树转为逻辑计划时各算子存在先后顺序。以select语句为例，执行的顺序为： 
FROM_CLAUSE.children.size()== 1 
走这个

FROM > WHERE > GROUP BY> HAVING > SELECT > DISTINCT > UNION > ORDER BY > LIMIT。 

1.
search_table_name(char * table_name,long long * no)
get_table_no_addr(long table_no,void ** addr); -> mem_table
mem_table->config->fields_table->index_nr【 relate_index	】 判断是否有索引
有索引-> ( mem_skiplist_index_scan_long,mem_hash_index_scan_long )
没索引-> ( compare_list , full_table_scan_with_conlist)

2.
__filter_stl
3.
__group_by_key_stl
4.
__filter_stl
5.
_map_stl
6.
_distinct_ctl
7.
_union_all_ctl or _union_ctl
8.
SORT
9.
暂无


没有优化的逻辑计划应按照上述顺序逐步生成或者逆向生成。转为逻辑计划算子则对应为： 
FROM_CLAUSE.children.size() > 1 
走这个

JOIN –> FILTER -> GROUP -> FILTER(HAVING) -> PROJECTION -> DIST -> UNION -> SORT -> LIMIT。
1.
FROM 1, FROM 2
2.
同上

*/
//  函数表
// _map_stl _filter_stl __group_by_key_stl  __reduce_stl  _distinct_ctl   _union_all_ctl   _union_ctl  __hash_inner_join_ctl   __intersect_ctl   
//    1          2              3                4             5                 6            7                   8                  9

//  g++ -w -std=c++11 -C pyh_plan.hpp 
//  g++ -w -std=c++11  pyh_plan.cpp  -o cc.exe  


//->QueryOptimiser
//0.检查表名是否存在
//1.检查常数条件快速短路
//2.根据表名 检查 单操作数不可再分条件 -> 检查字段是否存在 ->有无索引
//3.根据表名 检查 双操作数不可再分条件 -> 检查字段是否存在 ->有无索引
//拼成  comlist
//生成  条件简报
//
//无索引
//compare_list , full_table_scan_with_conlist
//有索引
// mem_skiplist_index_scan_long,mem_hash_index_scan_long )
 
 
 
 
#ifndef PLAN_TAB_HPP  
#define PLAN_TAB_HPP  
#include "query_analyser.hpp"
#include <functional>
#include <stdlib.h>
#include <algorithm>
#include <ctype.h>

#define PHYSICY_PLAN_EMPTY_TABLE 10401
#define FIELD_NAME_EMPTY 10402
#define ERR_COLUMN_NULL  10403
#define ERR_PLEASE_MARK_TABLE_COLOUM  10404
#define ERR_NOT_FOUND_COLOUM 10405
#define ERR_UNNOMAL_DOUBLE_CONDITION 10406
#define INFO_BROCKEN 10407
#define ERR_NOT_FOUND_TABLE 10408
#define ERR_JOIN_FIELD_TYPE_NOT_EQ 10409
#define ERR_NOT_FOUND_LINK 10410
template<int>
struct get_field_type
{
	
	typedef int value_type;
};

template<>
struct get_field_type<FIELD_TYPE_INT>
{
	
	typedef FIELD_INT value_type;
};

template<>
struct get_field_type<FIELD_TYPE_SHORT>
{
	
	typedef FIELD_SHORT value_type;
};

template<>
struct get_field_type<FIELD_TYPE_LONG>
{
	
	typedef FIELD_LONG value_type;
};

template<>
struct get_field_type<FIELD_TYPE_LONGLONG>
{
	
	typedef FIELD_LONGLONG value_type;
};

template<>
struct get_field_type<FIELD_TYPE_FLOAT>
{
	
	typedef FIELD_FLOAT value_type;
};

template<>
struct get_field_type<FIELD_TYPE_DOUBLE>
{
	
	typedef FIELD_DOUBLE value_type;
};

template<>
struct get_field_type<FIELD_TYPE_DATE>
{
	
	typedef FIELD_DATE value_type;
};

template<>
struct get_field_type<FIELD_TYPE_STR>
{
	
	typedef char* value_type;
};

class QueryPlan;


// 表中是否有该字段
inline int has_field(mem_table_t *mem_table ,std::string field_name){  
   
   if( NULL == mem_table        )   return PHYSICY_PLAN_EMPTY_TABLE;
   if( field_name.empty()       )   return FIELD_NAME_EMPTY;
   for(int i = 0;i < mem_table->field_used_num; ++i  )
   {
   		field_t& field = mem_table->fields_table[i];
   	  if (0 == strcmp(field.field_name,field_name.c_str()) ){
   	  	return 1;
   	  }
   }
   return 0;

}
// 表中是否有索引
inline int field_has_index(mem_table_t *mem_table ,std::string field_name,long & index_no,int & index_type){  
   int err = has_field( mem_table , field_name);
   if( !err )return err;
   	
   for(int i = 0;i < mem_table->field_used_num; ++i  )
   {
   		field_t& field = mem_table->fields_table[i];
   	  if (0 == strcmp(field.field_name,field_name.c_str()) ){
   	  	if(field.index_nr > 0 ){
   	  		index_no = field.relate_index[0];
   	  		index_type = field.index_type;
   	  		return 1;
   	  	}
   	  	
   	  }
   }
   return 0;

}

// 将结果行中的字段析出
template<class T,int T2>
get_field_type<T2>::value_type cast_field( T * record , const field_t & field ){  
    
    switch( field.field_type )
    {
    	case FIELD_TYPE_INT:            return *(FIELD_INT *)( (char *)(record)+field.field_dis );
			case FIELD_TYPE_SHORT:          return *(FIELD_SHORT *)( (char *)(record)+field.field_dis );
 			case FIELD_TYPE_LONG:           return *(FIELD_LONG *)( (char *)(record)+field.field_dis );
			case FIELD_TYPE_LONGLONG:       return *(FIELD_LONGLONG *)( (char *)(record)+field.field_dis );
			case FIELD_TYPE_FLOAT:          return *(FIELD_FLOAT *)( (char *)(record)+field.field_dis );
			case FIELD_TYPE_DOUBLE:         return *(FIELD_DOUBLE *)( (char *)(record)+field.field_dis );
 			case FIELD_TYPE_DATE:           return *(FIELD_DATE *)( (char *)(record)+field.field_dis );
 			//case FIELD_TYPE_HASH_ENTRY:       return *(FIELD_INT *)( (char *)(record)+field.field_dis ); 
 			//case FIELD_TYPE_RBTREE_ENTRY:     return *(FIELD_INT *)( (char *)(record)+field.field_dis );
 			//case FIELD_TYPE_SKIPLIST_ENTRY:   return *(FIELD_INT *)( (char *)(record)+field.field_dis );
 			case FIELD_TYPE_STR:            return *(char *)( (char *)(record)+field.field_dis );
    		
    }
   	
   return 0;
}

template<int T>
get_field_type<T>::value_type cast_condition( std::string& condition , const field_t & field ){  
    
    switch( field.field_type )
    {
    	case FIELD_TYPE_INT:            return  (FIELD_INT)atoi(condition.c_str() );
			case FIELD_TYPE_SHORT:          return  (FIELD_SHORT)atoi(condition.c_str() );
 			case FIELD_TYPE_LONG:           return  (FIELD_LONG )atol(condition.c_str() );
			case FIELD_TYPE_LONGLONG:       return  (FIELD_LONGLONG)atoll(condition.c_str() );
			case FIELD_TYPE_FLOAT:          return  (FIELD_FLOAT *)atof(condition.c_str() );
			case FIELD_TYPE_DOUBLE:         return  (FIELD_DOUBLE *)atof(condition.c_str() );
				
 			case FIELD_TYPE_DATE:           return  (FIELD_DATE )atol(condition.c_str() );
 			//case FIELD_TYPE_HASH_ENTRY:       return *(FIELD_INT *)( (char *)(record)+field.field_dis ); 
 			//case FIELD_TYPE_RBTREE_ENTRY:     return *(FIELD_INT *)( (char *)(record)+field.field_dis );
 			//case FIELD_TYPE_SKIPLIST_ENTRY:   return *(FIELD_INT *)( (char *)(record)+field.field_dis );
 			case FIELD_TYPE_STR:         		return  (char *)(condition.c_str() );
    		
    }
   	
   return 0;
}

// 将结果行中的字段析出
template<int T>
get_field_type<T>::value_type* cast_ptr_by_field( std:string const_value , const field_t & field ){  
    
    get_field_type<T>::value_type* ret = new get_field_type<T>::value_type;
    
    switch( field.field_type )
    {
    	case FIELD_TYPE_INT:             *ret = (FIELD_INT 	 		)( atoi( const_value.c_str() ) ); break;
			case FIELD_TYPE_SHORT:           *ret = (FIELD_SHORT 		)( atoi( const_value.c_str() ) ); break;
 			case FIELD_TYPE_LONG:            *ret = (FIELD_LONG  		)( atol( const_value.c_str() ) ); break;
			case FIELD_TYPE_LONGLONG:        *ret = (FIELD_LONGLONG )( atoll( const_value.c_str() ) ); break;
			case FIELD_TYPE_FLOAT:           *ret = (FIELD_FLOAT 		)( atof( const_value.c_str() ) ); break;
			case FIELD_TYPE_DOUBLE:          *ret = (FIELD_DOUBLE 	)( atof( const_value.c_str() ) ); break;
 			case FIELD_TYPE_DATE:            *ret = (FIELD_DATE 		)( atol( const_value.c_str() ) ); break;
 			//case FIELD_TYPE_HASH_ENTRY:       return *(FIELD_INT *)( (char *)(record)+field.field_dis );  break;
 			//case FIELD_TYPE_RBTREE_ENTRY:     return *(FIELD_INT *)( (char *)(record)+field.field_dis ); break;
 			//case FIELD_TYPE_SKIPLIST_ENTRY:   return *(FIELD_INT *)( (char *)(record)+field.field_dis ); break;
 			case FIELD_TYPE_STR:             *ret = (char *)( const_value.c_str()  ); break;
    		
    }
   	
   return ret;
}

// 将结果行中的字段析出
template<int T>
get_field_type<T>::value_type* cast_ptr_by_field( char *const_value  , const field_t & field ){  
    
    get_field_type<T>::value_type* ret = new get_field_type<T>::value_type;
    
    switch( field.field_type )
    {
    	case FIELD_TYPE_INT:             *ret = (FIELD_INT 	 		)( atoi( const_value ) ); break;
			case FIELD_TYPE_SHORT:           *ret = (FIELD_SHORT 		)( atoi( const_value ) ); break;
 			case FIELD_TYPE_LONG:            *ret = (FIELD_LONG  		)( atol( const_value ) ); break;
			case FIELD_TYPE_LONGLONG:        *ret = (FIELD_LONGLONG )( atoll( const_value ) ); break;
			case FIELD_TYPE_FLOAT:           *ret = (FIELD_FLOAT 		)( atof( const_value ) ); break;
			case FIELD_TYPE_DOUBLE:          *ret = (FIELD_DOUBLE 	)( atof( const_value) ); break;
 			case FIELD_TYPE_DATE:            *ret = (FIELD_DATE 		)( atol( const_value ) ); break;
 			//case FIELD_TYPE_HASH_ENTRY:       return *(FIELD_INT *)( (char *)(record)+field.field_dis );  break;
 			//case FIELD_TYPE_RBTREE_ENTRY:     return *(FIELD_INT *)( (char *)(record)+field.field_dis ); break;
 			//case FIELD_TYPE_SKIPLIST_ENTRY:   return *(FIELD_INT *)( (char *)(record)+field.field_dis ); break;
 			case FIELD_TYPE_STR:             *ret = (char *)( const_value  ); break;
    		
    }
   	
   return ret;
}

// 从表中获得字段
inline int get_field(mem_table_t *mem_table ,std::string field_name,field_t & field){  
   int err = has_field( mem_table , field_name);
   if( !err )return err;
   	
   for(int i = 0;i < mem_table->field_used_num; ++i  )
   {
   		field_t& _field = mem_table->fields_table[i];
   	  if (0 == strcmp(_field.field_name,field_name.c_str()) ){
 			 field = _field;
 			 return 0;
   	  	
   	  }
   }
   return NOT_FOUND_FIELD;

}

//常量条件解析结构体
//主要解析表名、列名，操作类型
struct const_condition_struct{
	
	int tag;
	std::string const_value;
  std::string const_type;
  rapidjson::Value *  context_ ;
	QueryPlan        *  query_plan_;
  
  const_condition_struct(rapidjson::Value *  context, QueryPlan * query_plan ):context_(context),query_plan_(query_plan){
		tag = (*context_)["tag"].GetInt();
		//直接获得常量
		if( (*context_)["children"][0].HasMember("str_value_") ){
			const_value = (*context_)["children"][0]["str_value_"].GetString();
		}
		if( (*context_)["children"][0].HasMember("CONST_TYPE") ){
			const_type = (*context_)["children"][0]["CONST_TYPE"].GetString();
		}
		
	}
	// 返回1 忽略该条件
	// 返回 INFO_BROCKEN 快速短路
	// 返回 0 保留
	int is_drop(){
		if(tag == T_BOOL && const_value == "1" )return 1;
		if(tag == T_BOOL && const_value == "0" )return INFO_BROCKEN;
		else return 0;
	}
	
}

//单条件解析结构体
//主要解析表名、列名，操作类型
struct normal_single_condition_struct{
	int tag;
	std::string relation_name;
	std::string column_name;
	rapidjson::Value *  context_ ;
	QueryPlan        *  query_plan_;
	int has_index;
	int index_type;
	long index_no;
	
	normal_single_condition_struct(rapidjson::Value *  context, QueryPlan * query_plan ):context_(context),query_plan_(query_plan),has_index(0){
		tag = (*context_)["tag"].GetInt();
		
		//直接获得表名
		if( (*context_)["children"][0].HasMember("RELATION_NAME") ){
			relation_name = (*context_)["children"][0]["RELATION_NAME"].GetString();
		}
		if( (*context_)["children"][0].HasMember("COLUMN_NAME") ){
			column_name = (*context_)["children"][0]["COLUMN_NAME"].GetString();
		}
		
	}
	
	bool operator < (const normal_single_condition_struct &a) const
	{
		return has_index < a.has_index;	
	}	
	
int get_name(){	
		if( 0 == column_name.size() ) 
			{
				// 未获取正常的列
				return ERR_COLUMN_NULL;
			}
			
			mem_table_t *mem_table = NULL;
		
		//直接获得表名失败,从tables 里找对应的relation_name，找不到或找到多个报错
		if( 0 == relation_name.size() ){
			int field_num = 0;
			for(auto & table : query_plan_->tables )
			{
				long long table_no;
				
				int err = 0;
				// 对于原始表检查该表是否存在
				if(!table.table_name_.empty()){
					 err = search_table_name(table.table_name_.c_str(),&table_no);
					
					 if( 0 == err){
					 	 err = get_table_no_addr(table_no,(void **)(&mem_table));
					 	 if( 0 == err){
					 	 		if( has_field( mem_table ,column_name ) ){
					 	 			++field_num;
					 	 			relation_name = std::string(mem_table->table_name);
					 	 		}
					 		}
					 }
					 
				}
			}
			
			if( 1 < field_num )return ERR_PLEASE_MARK_TABLE_COLOUM;
			if( 0 ==field_num )return ERR_NOT_FOUND_COLOUM; 	
				
			// 只找到1个对应的table里有列名
			if( 1 == field_num ){
				has_index = field_has_index(mem_table ,column_name,index_no,index_type);
				return 0;
			}
		}
		return 0;
		
		}
		
};

//二元条件解析结构体
//主要解析表名、列名，操作类型
struct normal_double_condition_struct:public normal_single_condition_struct{
  std::string const_value;
  std::string const_type;

		normal_double_condition_struct(rapidjson::Value *  context, QueryPlan * query_plan ):normal_single_condition_struct(context,query_plan){
	  }
	  
	 bool operator < (const normal_double_condition_struct &a) const
	{
		return has_index <= a.has_index;	
	}	
	  
int get_name(){	  
		int i = 0; // 代表有列名的序号
		int j = 0; // 代表有列名的序号
		
		if( (*context_)["children"][0].HasMember("COLUMN_NAME") && (*context_)["children"][1].HasMember("CONST_TYPE") ) {i = 0; j = 1;}
		else ( (*context_)["children"][1].HasMember("COLUMN_NAME") &&(*context_)["children"][0].HasMember("CONST_TYPE") ) {i = 1; j = 0;}
		else return ERR_UNNOMAL_DOUBLE_CONDITION;		
		
		//直接获得常量
		if( (*context_)["children"][j].HasMember("str_value_") ){
			const_value = (*context_)["children"][j]["str_value_"].GetString();
		}
		if( (*context_)["children"][j].HasMember("CONST_TYPE") ){
			const_type = (*context_)["children"][j]["CONST_TYPE"].GetString();
		}
		
		//直接获得表名
		if( (*context_)["children"][i].HasMember("RELATION_NAME") ){
			relation_name = (*context_)["children"][i]["RELATION_NAME"].GetString();
		}
		if( (*context_)["children"][i].HasMember("COLUMN_NAME") ){
			column_name = (*context_)["children"][i]["COLUMN_NAME"].GetString();
		}
		
		return normal_single_condition_struct::get_name();	
	}



};

//二元条件解析结构体
//主要解析表名、列名，操作类型
struct normal_btw_condition_struct:public normal_single_condition_struct{
  std::string const_value[2];
  std::string const_type;

		normal_btw_condition_struct(rapidjson::Value *  context, QueryPlan * query_plan ):normal_single_condition_struct(context,query_plan){
	  }
	  
	 bool operator < (const normal_double_condition_struct &a) const
	{
		return has_index < a.has_index;	
	}	
	  
int get_name(){	  
		
		int k = 0;
		for(int i = 0;i<3;++i)
		{
			//if( (*context_)["children"][i].HasMember("COLUMN_NAME") && (*context_)["children"][i].HasMember("CONST_TYPE") ) {btw_index[i] = 0;}
			//直接获得表名
			if( (*context_)["children"][i].HasMember("RELATION_NAME") ){
				relation_name = (*context_)["children"][i]["RELATION_NAME"].GetString();
			}
			if( (*context_)["children"][i].HasMember("COLUMN_NAME") ){
				column_name = (*context_)["children"][i]["COLUMN_NAME"].GetString();
			}
			//直接获得常量
			if( (*context_)["children"][i].HasMember("str_value_") ){
					const_value[k++] = (*context_)["children"][i]["str_value_"].GetString();
			}
				
		}
		if( const_value[0] > const_value[1] )std:swap(const_value[0],const_value[1]);
		
		return normal_single_condition_struct::get_name();	
	}



};

#define NEED_DELETE -1
// 等号关联条件
struct join_eq_condition_struct{
	int tag;
	std::string relation_name[2];
	std::string column_name[2];
	field_t field[2];
  mem_table_t *mem_table[2];
  
	rapidjson::Value *  context_ ;
	QueryPlan        *  query_plan_;
	int has_index[2];
	int index_type[2];
	long index_no[2];
	
	
	join_eq_condition_struct(rapidjson::Value *  context, QueryPlan * query_plan ):context_(context),query_plan_(query_plan){
		tag = (*context_)["tag"].GetInt();
		
		//直接获得表名
		if( (*context_)["children"][0].HasMember("RELATION_NAME") ){
			relation_name[0] = (*context_)["children"][0]["RELATION_NAME"].GetString();
		}
		if( (*context_)["children"][0].HasMember("COLUMN_NAME") ){
			column_name[0] = (*context_)["children"][0]["COLUMN_NAME"].GetString();
		}
				//直接获得表名
		if( (*context_)["children"][1].HasMember("RELATION_NAME") ){
			relation_name[1] = (*context_)["children"][1]["RELATION_NAME"].GetString();
		}
		if( (*context_)["children"][1].HasMember("COLUMN_NAME") ){
			column_name[1] = (*context_)["children"][1]["COLUMN_NAME"].GetString();
		}
		
	}
	
	// 有连接关系返回 1 
	// 重复条件返回 NEED_DELETE
	// 无关系返回 0
	inline int can_link_or_delete( join_eq_condition_struct & linker )
	{
		int i = 0;
		for(int i = 0; i<2; ++i){
			for(int j = 0; j<2 ; ++j){
				if( relation_name[i] == linker.relation_name[j] && column_name[i] == linker.column_name[j] )++i;
			}
		}
		if ( 1 == i )return 1;
		if ( 2 == i )return NEED_DELETE;
		else return 0;
		
	}
	// 获得连接 序号，self 是自己的，next是右边的
	inline int get_link_seq( join_eq_condition_struct & linker,int & self, int & next )
	{
		int k = 0;
		for(int i = 0; i<2; ++i){
			for(int j = 0; j<2 ; ++j){
				if( relation_name[i] == linker.relation_name[j] && column_name[i] == linker.column_name[j] )++k;
			}
		}
		if ( 1 == k ){ self = i; next = j ;return 1;}
		if ( 2 == i )return NEED_DELETE;
		else return 0;
		
	}
	
	bool operator < (const join_eq_condition_struct &a) const
	{
		return get_weight () < a.get_weight ();	
	}	
	
	//获得权重，权重越小优先级越高
		unsigned long get_weight ()
	{
		return std::min( mem_table_get_used_line( mem_table[0] ) , mem_table_get_used_line( mem_table[1] ) );
	}	
	
int get_name(){	
		if( 0 == column_name1.size() || 0 == column_name2.size() || 0 == relation_name1.size() || 0 == relation_name2.size() ) 
			{
				// 未获取正常的列
				return ERR_COLUMN_NULL;
			}
			
			mem_table[0] = NULL;
			mem_table[1] = NULL;

		//直接获得表名失败,从tables 里找对应的relation_name，找不到或找到多个报错
	
			int field_num = 0;
			int finded = 0;
			for(auto & table : query_plan_->tables )
			{
				
				for(int i = 0; i<2; ++i){
					if( relation_name[i] == table.alias_name_ )relation_name[i] = table.table_name_;
					if( relation_name[i] == table.table_name_ ) ++finded;
				}
				
			 }
			 if(finded<1)return ERR_NOT_FOUND_TABLE;
		
		  long long table_no;
			int err = 0;
			
		// 检查表中的字段是否存在
				for(int i = 0; i<2; ++i){
					 err = search_table_name(relation_name[i].c_str(),&table_no);
					
					 if( 0 == err){
					 	 err = get_table_no_addr(table_no,(void **)(&(mem_table[i])));
					 	 if( 0 == err){
					 	 		if( !has_field( mem_table[i] ,column_name[i] ) ){
					 	 			return ERR_NOT_FOUND_COLOUM; 	
					 	 		}
					 	 		else
					 	 			get_field( mem_table[i] , column_name[i], field_t &(field[i]) )
					 		}
					 }
					}
					
		if( field[0].field_type != field[1].field_type )return ERR_JOIN_FIELD_TYPE_NOT_EQ;		
		return 0;
		
		}
		
};




class QueryPlan : public QueryAnalyser
{
//是否快速短路
int is_broken;
// 对应每个单表的结果集
std::map(std::string,std::list<record_type>* ret_list) table_ret_name;

public:
QueryPlan(rapidjson::Value & value ,rapidjson::Document & doc_ , QueryAnalyser * up_stmt_ = NULL ):QueryAnalyser(value,doc_,up_stmt_),is_broken(0){}

QueryPlan(QueryAnalyser &qa ):QueryAnalyser( *(qa.root),*(qa.doc),NULL),is_broken(0){}

// 查询分类
int queryPlanClassify(){
	
//if(doc->)
if( 1 == tables.size() )
{
	return querySimpalPlan(0);
}
else if ( 1 < tables.size() )
{
	return queryComplexPlan();
}


}

 // 获得单个表的扫描过滤条件
 // 符合加入到 compare_list 条件的 normal_single_condition
 // 符合加入到 compare_list 条件的 normal_double_condition
 // 先单表过滤再 join
int getSinglTableFilter(
											/*in*/  int i,
											/*out*/ mem_table_t **mem_table
  										/*out*/ std::list<normal_single_condition_struct>& normal_single_condition_list,
  										/*out*/ std::list<normal_double_condition_struct>& normal_double_condition_list,
  										/*out*/ std::list<normal_double_condition_struct>& normal_index_double_condition_list,											
											/*out*/ std::list<const_condition_struct>&				 const_condition_list
										)
{
	if( mem_table == NULL) return PHYSICY_PLAN_EMPTY_TABLE
	string table_name;  // 表名
  string alias_name;  // 表别名
  string sub_select_alias_name; //子查询表别名
  
  if( (*v).HasMember("RELATION") ) 				 table_name= tables[i].table_name_; 	// 表名				
  if( (*v).HasMember("RELATION_ALIAS") )	 alias_name= tables[i].alias_name_		//表别名
  if( (*v).HasMember("SUB_SELECT_ALIAS") ) sub_select_alias_name=tables[i].sub_select_alias_name_; //子查询表别名
  
  long long table_no;
  int err = 0;

  // 对于原始表检查该表是否存在
  if(!table_name.empty()){
  	 err = search_table_name(table_name.c_str(),&table_no);
  	}
  	else if(!sub_select_alias_name.empty()){
  		// 递归处理子查询的逻辑
  	}
  	
  	if ( err == 0 ){// 找到该表的ID
  			
  			err = get_table_no_addr( table_no,(void **)(mem_table) );
  			if( err == 0 ){ // 找到该表的地址
  				
  				for(auto &nomal_single_condition : nomal_single_conditions ){//查找单条件列表中，使用该表字段的一元条件
  					normal_single_condition_struct nscs(nomal_single_condition,this);
  					err = nscs.get_name();
  					if( err == 0 && 0 == strcmp( nscs.relation_name.c_str(),(*mem_table)->table_name ) ){
  						  normal_index_single_condition_list.push_back(nscs);
  					}
  				}
  				if(!err)return err;
  				//按是否有索引排列
  				normal_single_condition_list.sort();
  				
  				for(auto &nomal_double_condition : nomal_double_conditions ){//查找双条件列表中，使用该表字段的二元条件
  					nomal_double_condition_struct ndcs(nomal_double_condition,this);
  					err = ndcs.get_name();
  					if( err == 0 && 0 == strcmp( ndcs.relation_name.c_str(),(*mem_table)->table_name ) ){
  						normal_double_condition_list.push_back(ndcs);
  						  if(nscs.has_index) normal_double_condition_list.push_back(ndcs);
  						 	else 							 normal_index_double_condition_list.push_back(ndcs);
  					}
  				}
  				if(!err)return err;
  				//按是否有索引排列
  				normal_double_condition_list.sort();
  					
  				for(auto &const_condition : const_conditions ){//查找常量条件列表中，判断是否短路
  					const_condition_struct ccs(const_condition,this);
  					err = ccs.is_drop();
  					if( err == 0   ){
  						const_condition_list.push_back(ccs);
  					}
  					else if(err == INFO_BROCKEN ){
  						if_broken = 1;
  						break;
  					}
  				}
  				if(!err)return err;
  					
  				
  				
  			
  			}
  		}
  }

// 由于是内存数据库，暂时不打算支持外部排序，为节约内存
// 因此对于各表，优先用条件进行过滤
// 过滤后的结果尽量小，再进行关联
 
int getSinglTableSource(
											/*in */ mem_table_t *mem_table
  										/*out*/ std::list<normal_single_condition_struct>& normal_single_condition_list,
  										/*out*/ std::list<normal_double_condition_struct>& normal_double_condition_list,
  										/*out*/ std::list<normal_double_condition_struct>& normal_index_double_condition_list
  										/*out*/ std::list<plan_node *>									 & plan_node_list
										)
{
	if( mem_table == NULL )return PHYSICY_PLAN_EMPTY_TABLE;
  
  //默认关闭索引扫描，开启普通原表扫描
	int is_indxe_scan = 0;
	int is_origin_scan = 1;
	int err = 0;
	
  // 只有双条件支持索引
	if( normal_index_double_condition_list.size()>0 ){
		is_indxe_scan = 1;
		is_origin_scan = 0;
		
		plan_node * node ;
		err = handl_double_con_index( mem_table , 
														normal_index_double_condition_list,
														normal_double_condition_list,
														plan_node_list
													);
		//if(!err)plan_node_list.push_back(node);
	}
	
	
	// 双条件索引后先进行普通双条件过滤
	if( normal_double_condition_list.size()>0 ){
		
		if(is_indxe_scan){ //在索引结果上filter
			
					plan_node * node ;
					err = handl_double_con_from_index( ret , 
														mem_table
														normal_double_condition_list,
														plan_node_list 
													);
		//if(!err)plan_node_list.push_back(node);

		}
		else //原表扫描
			{
				 	plan_node * node ;
					err = handl_double_con_normal( mem_table , 
														normal_double_condition_list,
														plan_node_list 
													);
		//if(!err)plan_node_list.push_back(node);

			}
		is_origin_scan = 0;
	}
	if( normal_single_condition_list.size()>0 ){
		if( !is_origin_scan ){ //在索引结果上filter
			
			  plan_node * node ;
			 	err = handl_single_con_from_index( &((plan_node_list.end()-1)->ret_list) , 
															normal_single_condition_list,
															plan_node_list
														);
				//if(!err)plan_node_list.push_back(node);

		}
		else //原表扫描
			{
				
				plan_node * node ;
				err = handl_single_con_normal( mem_table , 
															normal_single_condition_list,
															plan_node_list  
														);
			//	if(!err)plan_node_list.push_back(node);

			}
	}
	
	 //无条件全表扫
if( normal_single_condition_list.size() == 0 &&  normal_double_condition_list.size() == 0 && normal_index_double_condition_list == 0 )
{
		    plan_node * node ;
				err = handl_no_con_normal( mem_table , 
															plan_node_list 
														);
			//	if(!err)plan_node_list.push_back(node);
}

// 将该表的结果级插入到 map
table_ret_name.insert(std::pair<std::string,std::list<record_type>*>(std::string(mem_table->table_name ), (plan_node_list.end()-1 )->ret_list ) );

											
}


//// 双条件索引
int handl_double_con_index( mem_table_t *mem_table , 
														std::list<normal_double_condition_struct> & index_list,
														std::list<normal_double_condition_struct> & nomal_list,
														 std::list<plan_node *>									  & plan_node_list
													  )
{
int err = 0;
int first = 1;
std::shared<finded_Addr_t> finded_Addr_t (new finded_Addr );
finded_Addr_t *pre_Addr = finded_Addr.get();
for(auto& v : index_list)
{
  int tag = v.tag;
  long index_no = v.index_no;
  int  index_type = v.index_type; // 1 hash 2 skiplist
  void * addr;
  
  err = get_index_no_addr( index_no,&addr);
  if(!err)return err;
  	
	std::string relation_name = v.relation_name;
	std::string column_name   = v.column_name;
	std::string const_value   = v.const_value;
  std::string const_type    = v.const_type;

	plan_node * node;  
	
if( INDEX_TYPE_HASH == index_type)
{
mem_hash_index_t * index = (mem_hash_index_t *) addr;
switch(tag)
{
case T_OP_EQ :{
node = new scan_index_hash_node(index_no,
  								               index_type,
  								               mem_table,
  								               index,
  								               v->context_,
  								               &(v->Doc),
  								               relation_name,
  								               column_name,
  								               const_value,
  								               const_type,
  								               pre_Addr);
pre_Addr = &(node->ret_list);
plan_node_list.push_back(node);

break;
}   
  default :// hash 索引对等值以外的查询不处理，交给全表scan
  nomal_list.push_back(v);
  }
}

if( INDEX_TYPE_SKIP == index_type)
{
	mem_skiplist_index_t *index = (mem_skiplist_index_t *) addr;
  switch(tag)
  {
  case T_OP_EQ :{
	node = new scan_skiplist_eq_node(index_no,
	  								              	index_type,
	  								              	mem_table,
	  								              	index,
	  								              	v->context_,
	  								              	&(v->Doc),
	  								              	relation_name,
	  								              	column_name,
	  								              	const_value,
	  								              	const_type,
	  								              	pre_Addr);
	pre_Addr = &(node->ret_list);

	plan_node_list.push_back(node);
	
	break;
}      
  case T_OP_LE :{
	node = new scan_skiplist_le_node(index_no,
	  								              	index_type,
	  								              	mem_table,
	  								              	index,
	  								              	v->context_,
	  								              	&(v->Doc),
	  								              	relation_name,
	  								              	column_name,
	  								              	const_value,
	  								              	const_type,
	  								              	pre_Addr);
	pre_Addr = &(node->ret_list);
	
	plan_node_list.push_back(node);
	break;
}  
  case T_OP_LT :{
	node = new scan_skiplist_lt_node(index_no,
	  								              	index_type,
	  								              	mem_table,
	  								              	index,
	  								              	v->context_,
	  								              	&(v->Doc),
	  								              	relation_name,
	  								              	column_name,
	  								              	const_value,
	  								              	const_type,
	  								              	pre_Addr);
  pre_Addr = &(node->ret_list);
	plan_node_list.push_back(node);
	break;
}  
  case T_OP_GE :{
	node = new scan_skiplist_ge_node(index_no,
	  								              	index_type,
	  								              	mem_table,
	  								              	index,
	  								              	v->context_,
	  								              	&(v->Doc),
	  								              	relation_name,
	  								              	column_name,
	  								              	const_value,
	  								              	const_type,
	  								              	pre_Addr
	  								              	);
  pre_Addr = &(node->ret_list);
	plan_node_list.push_back(node);
	break;
}  
  case T_OP_GT :{
	node = new scan_skiplist_gt_node(index_no,
	  								              	index_type,
	  								              	mem_table,
	  								              	index,
	  								              	v->context_,
	  								              	&(v->Doc),
	  								              	relation_name,
	  								              	column_name,
	  								              	const_value,
	  								              	const_type,
	  								              	pre_Addr);
	pre_Addr = &(node->ret_list);
	plan_node_list.push_back(node);
	break;
}  
//  case T_OP_BTW: {
//	*node = new scan_skiplist_btw_node(index_no,
//	  								              	index_type,
//	  								              	mem_table,
//	  								              	index_min,
//	  								              	index_max,
//	  								              	v->context_,
//	  								              	&(v->Doc),
//	  								              	relation_name,
//	  								              	column_name,
//	  								              	const_value,
//	  								              	const_type);
//	
//	break;
//}  
  case T_OP_NE :
  	
  default :// 其他情况，交给全表scan
  nomal_list.push_back(v);
  
  }
}

}

// 合并index 结果集
node = new merg_index_node(        	mem_table,
	  								              	index,
	  								              	v->context_,
	  								              	&(v->Doc),
	  								              	pre_Addr
	  								              	);
plan_node_list.push_back(node);
}					


				
//// 双条件过滤 源数据不是原表
template<class T>
int handl_double_con_from_index( T* container, // 数据源容器 stl
														mem_table_t *mem_table , 
														std::list<normal_double_condition_struct> & index_list,
														std::list<plan_node *>									  & plan_node_list
													  )
{

int err = 0;
for(auto& v : index_list)
{
  int tag = v.tag;
	std::string relation_name = v.relation_name;
	std::string column_name   = v.column_name;
	std::string const_value   = v.const_value;
  std::string const_type    = v.const_type;
  	
  field_t  field;
  err = get_field(mem_table ,column_name, field);
  if( err != 0 )return err;
  plan_node * node;  

  	
  	
  switch(tag)
  {
  case T_OP_EQ :{
	node = new filter_eq_node(container,
	  								              	field,
	  								             v->context_,
  								               &(v->Doc),
  								               relation_name,
  								               column_name,
	  								             const_value,
	  								             const_type);
	plan_node_list.push_back(node);
	break;
}      
  case T_OP_LE :{
	node = new filter_le_node(container,
	  								              	field,
	  								             v->context_,
  								               &(v->Doc),
  								               relation_name,
  								               column_name,
	  								             const_value,
	  								             const_type);
	plan_node_list.push_back(node);
	break;
}  
  case T_OP_LT :{
	node = new filter_lt_node(container,
	  								              	field,
	  								             v->context_,
  								               &(v->Doc),
  								               relation_name,
  								               column_name,
	  								             const_value,
	  								             const_type);
	plan_node_list.push_back(node);
	break;
}  
  case T_OP_GE :{
	node = new filter_ge_node(container,
	  								              	field,
	  								             v->context_,
  								               &(v->Doc),
  								               relation_name,
  								               column_name,
	  								             const_value,
	  								             const_type);
	plan_node_list.push_back(node);
	break;
}  
  case T_OP_GT :{
	node = new filter_gt_node(container,
	  								              	field,
	  								             v->context_,
  								               &(v->Doc),
  								               relation_name,
  								               column_name,
	  								             const_value,
	  								             const_type);
	plan_node_list.push_back(node);
	break;
}   
  case T_OP_NE :{
	node = new filter_ne_node(container,
	  								              	field,
	  								             v->context_,
  								               &(v->Doc),
  								               relation_name,
  								               column_name,
	  								             const_value,
	  								             const_type);
	plan_node_list.push_back(node);
	break;
}   
  
  case T_OP_LIKE :{
	node = new filter_like_node(container,
	  								              	field,
	  								             v->context_,
  								               &(v->Doc),
  								               relation_name,
  								               column_name,
	  								             const_value,
	  								             const_type);
	plan_node_list.push_back(node);
	break;
}  
  	
  default :// 其他情况，交给全表scan
  //nomal_list.push_back(v);
  
  }
  



}
return 0;
}

//_______________________________________________________
// 双条件过滤原表
int handl_double_con_normal(mem_table_t * mem_table , 
														std::list<normal_double_condition_struct>& normal_double_condition_list,
														std::list<plan_node *>								   & plan_node_list
													)
{
int err = 0;
compare_list * pre = NULL;
plan_node * node;  

for(auto& v : normal_double_condition_list)
{
  int tag = v.tag;
	std::string relation_name = v.relation_name;
	std::string column_name   = v.column_name;
	std::string const_value   = v.const_value;
  std::string const_type    = v.const_type;
  	
  field_t  field;
  err = get_field(mem_table ,column_name, field);
  if( err != 0 )return err;
  
  switch(tag)
  {
  case T_OP_EQ :{
	//*node = new filter_eq_node(container,
	//  								              	field,
	//  								             v->context_,
  //								               &(v->Doc),
  //								               relation_name,
  //								               column_name,
	//  								             const_value,
	//  								             const_type);
	 
	 std::shared_ptr<compare_list> cmp1(new compare_list);
	 cmp1->mem_table       			= mem_table;
	 cmp1->field_name      			= field.field_name;	 
	 
	 if(pre)pre->next					 	= cmp1.get();
	 pre = cmp1.get();
	 // 后面要记着 delete掉 cmp_field_value
	 cmp1->cmp_field_value 			=  cast_ptr_by_field<field.field_type>( const_value ,  field );

	 
	break;
}      
  case T_OP_LE :{
	 std::shared_ptr<compare_list> cmp1(new compare_le_list);
	 cmp1->mem_table       			= mem_table;
	 cmp1->field_name      			= field.field_name;	 
	 
	 if(pre)pre->next					 	= cmp1.get();
	 pre = cmp1.get();
	 // 后面要记着 delete掉 cmp_field_value
	 cmp1->cmp_field_value 			=  cast_ptr_by_field<field.field_type>( const_value ,  field );
	
	break;
}  
  case T_OP_LT :{
	 std::shared_ptr<compare_list> cmp1(new compare_lt_list);
	 cmp1->mem_table       			= mem_table;
	 cmp1->field_name      			= field.field_name;	 
	 
	 if(pre)pre->next					 	= cmp1.get();
	 pre = cmp1.get();
	 // 后面要记着 delete掉 cmp_field_value
	 cmp1->cmp_field_value 			=  cast_ptr_by_field<field.field_type>( const_value ,  field );
	
	break;
}  
  case T_OP_GE :{
	 std::shared_ptr<compare_list> cmp1(new compare_ge_list);
	 cmp1->mem_table       			= mem_table;
	 cmp1->field_name      			= field.field_name;	 
	 
	 if(pre)pre->next					 	= cmp1.get();
	 pre = cmp1.get();
	 // 后面要记着 delete掉 cmp_field_value
	 cmp1->cmp_field_value 			=  cast_ptr_by_field<field.field_type>( const_value ,  field );
	
	break;
}  
  case T_OP_GT :{
	 std::shared_ptr<compare_list> cmp1(new compare_gt_list);
	 cmp1->mem_table       			= mem_table;
	 cmp1->field_name      			= field.field_name;	 
	 
	 if(pre)pre->next					 	= cmp1.get();
	 pre = cmp1.get();
	 // 后面要记着 delete掉 cmp_field_value
	 cmp1->cmp_field_value 			=  cast_ptr_by_field<field.field_type>( const_value ,  field );
	
	break;
}   
  case T_OP_NE :{
	 std::shared_ptr<compare_list> cmp1(new compare_ne_list);
	 cmp1->mem_table       			= mem_table;
	 cmp1->field_name      			= field.field_name;	 
	 
	 if(pre)pre->next					 	= cmp1.get();
	 pre = cmp1.get();
	 // 后面要记着 delete掉 cmp_field_value
	 cmp1->cmp_field_value 			=  cast_ptr_by_field<field.field_type>( const_value ,  field );
	
	break;
}   
    case T_OP_LIKE :{
// 暂未实现
	
	break;
}  
  	
  default :// 其他情况，交给全表scan
  //nomal_list.push_back(v);
  
  }	
  	
  	if(pre != NULL){
  		node = new scan_normal_node(
  															mem_table,
	  								            field,
	  								            pre,//compare_list * pre
	  								            v->context_,
  								              &(v->Doc),
  								              relation_name,
  								              column_name,
	  								            const_value,
	  								            const_type);
	  								}
	 	plan_node_list.push_back(node);

  	
  	
  }

return 0;
}

//_______________________________________________________
// 单条件过滤 源数据不是原表
template<class T>
int handl_single_con_from_index( T* container, // 数据源容器 stl
															std::list<normal_single_condition_struct>& normal_single_condition_list,
															std::list<plan_node *>								   & plan_node_list
														)
{
	
	return -1;
}														

int handl_single_con_normal( mem_table_t * mem_table , 
															std::list<normal_single_condition_struct>& normal_single_condition_list,
															std::list<plan_node *>								   & plan_node_list
 	 	 	 	 	 	 	 	 	 	 ) 
{
	
	return -1;
}	


//0 排序
//1 先用最少的关联关系 去关联 剩下的能关联的最小关联关系  min(last.filter)
//2 再用已关联的 第二少的关联关系 去关联 剩下的能关联的最小关联关系
//3 再用已关联的 第3少的关联关系 去关联 剩下的能关联的最小关联关系
//4 剩下的关联为0 ，成功，否则失败

// 对关联条件进行排序
int order_join_condtions(std::list<join_eq_condition_struct> &join_eq_condition_origin)// 原始关联条件
{	
	
// 构造 原始关联条件
for( auto v : join_conditions )
{
join_eq_condition_struct join_eq_condition;

if( (*v)["tag"].GetInt() == T_OP_EQ )
{
join_eq_condition.context_ = v;
join_eq_condition.get_name();
join_eq_condition_origin.push_back(join_eq_condition_struct);
}
}

// 先按高水位线排序
join_eq_condition_origin.sort();
// 先用最少的关联关系 去关联 剩下的能关联的最小关联关系  min(last.filter)
for(std::list<join_eq_condition_struct>::iterator iter= join_eq_condition_origin.begin();
	iter!=join_eq_condition_origin.end()-1;++iter
)
{
	for(std::list<join_eq_condition_struct>::iterator iter2 = iter+1;
		iter2!=join_eq_condition_origin.end();++iter2)
		{
			int ret = iter->can_link_or_delete(*iter2);
			switch(ret){
				case NEED_DELETE:continue; //先不处理重复的
				case 0:continue;
				case 1:{ // 交换位置
					if(iter2 == iter+1)continue;
						else {
							std::swap( *(iter+1),*iter2 );
						}
					
				}
				
			}
		}
		
		if( 1 != iter->can_link_or_delete( *(iter+1)) )return ERR_NOT_FOUND_LINK;
	
}

return 0;
}




//int do_join_condtions(std::list<join_eq_condition_struct> &join_eq_condition_origin,std::list<generic_result> * ret_list )// 原始关联条件
//{	
//
//std::string table_name1 = join_eq_condition_origin.relation_name[0];
//std::string table_name2 = join_eq_condition_origin.relation_name[1];
//std::string column_name1 = join_eq_condition_origin.column_name[0];
//std::string column_name2 = join_eq_condition_origin.column_name[1];
//	
//mem_table_t *mem_table1  = join_eq_condition_origin.mem_table[0];
//mem_table_t *mem_table2  = join_eq_condition_origin.mem_table[1];
//
//std::list<generic_result>* ret_list1 =  table_ret_map[table_name1];
//std::list<generic_result>* ret_list2 =  table_ret_map[table_name2];
//
//field_t field1;
//get_field(mem_table1 ,column_name1, field1);
//char buf[field1.field_size+1];
//buf1[field1.field_size+1];	
//buf1[field1.field_size]='\0';
//
//field_t field2;
//get_field(mem_table2 ,column_name2, field2);
//char buf[field2.field_size+1];
//buf2[field2.field_size+1];	
//buf2[field2.field_size]='\0';
//
//
//shared_ptr<std::list<generic_result> > first_ret =ret_hash_inner_join_ctl(*ret_list1,*ret_list2,
//	[&](record_type& x){	
//	memcpy(buf1,x.get_date(),field1.field_size);
//	return *cast_ptr_by_field<field1.field_no>( buf1 , field1 );
//	},
//	[&](record_type& x){
//	memcpy(buf2,x.get_date(),field2.field_size);
//	return *cast_ptr_by_field<field2.field_no>( buf2 , field2 );
//	});
//	
//shared_ptr<std::list<generic_result> > pre = first_ret;
//	
//for( std::list<join_eq_condition_struct>::iterator iter =join_eq_condition_origin.begin()+1;iter!=join_eq_condition_origin.end() ;++iter )
//{
//std::string table_name1 = iter->relation_name[0];
//std::string table_name2 = iter->relation_name[1];
//std::string column_name1 = iter->column_name[0];
//std::string column_name2 = iter->column_name[1];
//mem_table_t *mem_table1  = iter->mem_table[0];
//mem_table_t *mem_table2  = iter->mem_table[1];
//std::list<generic_result>* ret_list1 =  table_ret_map[table_name1];
//std::list<generic_result>* ret_list2 =  table_ret_map[table_name2];
//// 关联的字段不是通用的
//field_t field1;
//get_field(mem_table1 ,column_name1, field1);
//char buf[field1.field_size+1];
//buf1[field1.field_size+1];	
//buf1[field1.field_size]='\0';
//
//field_t field2;
//get_field(mem_table2 ,column_name2, field2);
//char buf[field2.field_size+1];
//buf2[field2.field_size+1];	
//buf2[field2.field_size]='\0';
//
//pre =ret_hash_inner_join_ctl(*pre,*ret_list2,
//	[&](record_type& x){	
//	memcpy(buf1,x.get_date()+x.get_row_size() - mem_table1->record_size + field1.field_dis,field1.field_size);
//	return *cast_ptr_by_field<field1.field_no>( buf1 , field1 );
//	},
//	[&](record_type& x){
//	memcpy(buf2,x.get_date(),field2.field_size);
//	return *cast_ptr_by_field<field2.field_no>( buf2 , field2 );
//	});
//
//}
//ret_list = pre.get();
//return 0;
//}



int handl_join_condtions(std::list<plan_node *>		& plan_node_list)
{
std::list<join_eq_condition_struct>  join_eq_condition_struct_list;

for(auto v : join_conditions )
{
  join_eq_condition_struct_list.emplace_back( v.context, this );
  
}
		
int ret = order_join_condtions( join_eq_condition_struct_list );// 原始关联条件
if(!ret)return ret;

plan_node * node;  
int is_first = 1;
shared_ptr<std::list<generic_result> > pre = new (<std::list<generic_result> );
for(auto & v : join_eq_condition_struct_list)
{
		if(!is_first) pre = *( plan_node_list.end() -1  ).ret_list;
		node = new do_join_node(
  															v,
  															pre,
	  								            v->context_,
  								              &(v->Doc)
  								              is_first);
  	plan_node_list.push_back(node);							            
		if( is_first == 1)is_first = 0;
}
		
	
	
}


};


#endif	 	 	 	 	 	 	 	 	 
