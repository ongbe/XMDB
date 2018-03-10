#ifndef RECORD_DML
#define RECORD_DML
#include"../mem_date_index_ctl/mem_table_mvcc_op.h"
#include "record_tuple.h"

/*

 g++ -C -w -std=c++11 record_dml_t.h


*/


#define RECORD_PTR_IS_NULL  70000
#define MEMTABL_PTR_IS_NULL 70001

#define RECORD_KEY_INT_TYPE  70003
#define RECORD_KEY_STR_TYPE  70004

struct record_dml_t
{
mem_table_t *mem_table; // �Ǹ��������
record_meta       meta;
record_tuple tuple_one;

std::vector<field_t>     field_vector;
std::vector<std::string> field_names; // ���������ֶ���
std::vector<std::string> field_values; // ���������ֶ���

generic_result return_record;

//��ָ��
//�ֶ�������
//�ֶ�ֵ����
//��Ҫ������ insert into table ( xx,xx,xx ) values (xx,xx,xx);
//�� update talbe set xx = xx, xx=xx where ;
record_dml_t( mem_table_t *_mem_table, 
							std::vector<std::string>& _field_names,
							std::vector<std::string>& _field_values):field_names(_field_names),
																											 mem_table(_mem_table),
																											 field_values(_field_values)
{
		init();  	
}

record_dml_t( mem_table_t *_mem_table):mem_table(_mem_table)
{
		init();  	
}

inline void init()
{
	
		meta.from_table( mem_table );
		tuple_one.meta = &meta;
		tuple_one.result = &return_record;
		return_record.allocate(mem_table->record_size - RECORD_HEAD_SIZE);		
		
		field_t field;
		for( size_t i = 0; i< field_names.size() ;  ++i )
		{
			tuple_one.get_field_desc( mem_table ,field_names[i], field );
		}
		field_vector.emplace_back(field);
  	
}

// ��ֵ����� tuple ��
inline int fill_record( )
{
	int ret = 0;
  char * addr = NULL;  
	for( size_t i = 0; i< field_names.size() ;  ++i )
  {
  ret = tuple_one.set_field( field_names[i] , (char *)cast_ptr_by_field( field_values[i] , field_vector[i] ) );
  if( ret )return ret;
  }
  
  return 0;
}

// ����ʵʱ����ʱ������
//��Ҫ������ insert into table ( xx,xx,xx ) values (xx,xx,xx);
inline int insert_one_into_table(
												record_t ** out_record_ptr,	
												unsigned long long Tn  )
{
fill_record();	
long block_no;
return mem_mvcc_insert_record( mem_table ,
                          out_record_ptr,
                          &block_no, /* in */
                          return_record.get_data(),
                          Tn        //����ID
                          );

}

// ����ʵʱɾ��ʱɾ����
inline int delete_one_from_table(
												record_t * in_record_ptr,	
												unsigned long long Tn  )
{
return mem_mvcc_delete_record( mem_table ,
																				in_record_ptr,
																				Tn               // ������ID
																				);
}
// ����ʵʱ����ʱ��������
//��Ҫ������ //�� update talbe set xx = xx, xx=xx where ;
inline int update_one_from_table(
												record_t * in_record_ptr,	
												record_t ** out_record_ptr,	
												unsigned long long Tn  )
{
fill_record( );	
return mem_mvcc_update_record( mem_table ,
															in_record_ptr,
															return_record.get_data(),
															Tn,               // ������ID
															out_record_ptr
															);
}

};




#endif