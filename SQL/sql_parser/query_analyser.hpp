#ifndef QYAN_TAB_HPP  
#define QYAN_TAB_HPP  
#include "parser.hh"
#include<vector>


void rapidjson_log(rapidjson::Value  * v)
	{
		 rapidjson::StringBuffer buffer;
     rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
		 rapidjson::Document    Doc; 
		 Doc.SetObject().AddMember("LOG",*v, Doc.GetAllocator() );
		 
		
		 Doc.Accept(writer);
		 
		  std::cout << buffer.GetString() << std::endl;
	}
	
	void rapidjson_log(rapidjson::Value  & v)
	{
		rapidjson_log(&v);
	}


 struct SelectItem
    {
      //uint64_t   expr_id_;
      bool       is_real_alias_;
      string     alias_name_;
      string     expr_name_;
      int        type_;
    };

 struct OrderItem
    {
      enum OrderType
      {
        ASC,
        DESC
      };

      //uint64_t   expr_id_;
      OrderType  order_type_;
    };

    struct JoinedTable
    {
      enum JoinType
      {
        T_FULL,
        T_LEFT,
        T_RIGHT,
        T_INNER,
      };
    };
        
struct TableItem  
{  
    string    table_name_;  
    string    alias_name_;  
    TableItem(string & table_name , string & alias_name ):table_name_(table_name),alias_name_(alias_name){}
};  


#define check_and_assign_member( target, _member ) \
if(  root->HasMember( _member ) )					 \
		{                                                 		\
			(target) = &( (*root)[ _member ] );			\
		}																						\
		else cout<<"do not have "<<_member<<endl;



class QueryAnalyser
{
rapidjson::Value    *root;
rapidjson::Document *doc;

int is_distinct;
int is_join;
int sub_links_count;
// ԭʼҪ��
rapidjson::Value  * project_lists;
rapidjson::Value  * from_list;
rapidjson::Value  * where_list;
rapidjson::Value  * group_list;
rapidjson::Value  * having_list;
rapidjson::Value  * order_list;
rapidjson::Value  * hints_list;



//����ʶ�����Ҫ��
vector<rapidjson::Value *> sub_querys;  // from  �е��Ӳ�ѯ
						
vector<TableItem>     tables;				 					// ������ı�
vector<QueryAnalyser> sub_links; 	   					// where �е�������
rapidjson::Value  *   up_where_list; 					//����������� ��Ҫ��һ���ϲ� where
vector<rapidjson::Value *> join_conditions;   // ��������

vector<rapidjson::Value *> const_conditions;         	// ���������ٷ�����
vector<rapidjson::Value *> nomal_single_conditions;  	// �������������ٷ�����
vector<rapidjson::Value *> nomal_double_conditions;  	// ˫�����������ٷ�����
vector<rapidjson::Value *> complex_double_conditions; // ���ٷ�˫����������
vector<rapidjson::Value *> nomal_btw_conditions;      // between and 
private:

    // �Ƿ��������� 
    int check_if_sub_query_link(int tag)
    {
    	if(tag == T_OP_EXISTS || tag == T_OP_IN || tag == T_OP_NOT_IN )
    	{
    	  return 1;
    	}
    	return 0;
    }	
    
     // �Ƿ����߼����� 
    int check_if_logic_predication(int tag)
    {
    	switch(tag)
    	{
    		case  T_OP_AND  :
    	  case  T_OP_OR   :   
    	  return 1;
    	}
    	return 0;
    }	
    
     // �Ƿ��ǳ���ֵ 
    int check_if_const_value(int tag)
    {
    	if( (tag>= T_INT  && tag <= T_UNKNOWN) || (tag>= T_TYPE_INTEGER  && tag <= T_TYPE_MODIFYTIME) )
    	{
    	  return 1;
    	}
    	return 0;
    }	
    
    // �Ƿ��ǹ������� 
     int check_if_join_condition(int tag)
    {
    	if( tag == T_OP_NAME_FIELD  || tag == T_IDENT )
    	{
    	  return 1;
    	}
    	return 0;
    }	
    
        // �Ƿ��ǹ������� 
     int check_if_name_filed(int tag)
    {
    	if( tag == T_OP_NAME_FIELD   )
    	{
    	  return 1;
    	}
    	return 0;
    }	
    
 
      // �Ƿ��ǹ������� 
     int check_if_obvious_join(int tag)
    {
    	if( tag >= T_JOIN_INNER  && tag <= T_JOIN_RIGHT )
    	{
    	  return 1;
    	}
    	return 0;
    }	
  
   // �Ƿ��ǾۺϺ��� 
     int check_if_aggregation_fun(int tag)
    {
    	if( ( tag >= T_FUN_MAX  && tag == T_FUN_AVG ) )
    	{
    	  return 1;
    	}
    	return 0;
    }	
    // �Ƿ���ʱ�亯�� 
     int check_if_time_fun(int tag)
    {
    	if(  tag >= T_CUR_TIME  && tag <= T_CUR_TIME_OP )  
    	{
    	  return 1;
    	}
    	return 0;
    }	
    
void swap_father(rapidjson::Value *father, rapidjson::Value *member, int tag ,const char * type_name ){
 		 	rapidjson::Value  *val_father ;
		  val_father = new rapidjson::Value;
		  rapidjson::Value val_tag;
		  val_tag.SetInt(tag);	
		  val_father->SetObject().AddMember("tag", val_tag, doc->GetAllocator() ).AddMember( rapidjson::StringRef( type_name ) ,*member, doc->GetAllocator() ) ;
		  father->Swap(*val_father);
}
	
public:
QueryAnalyser(rapidjson::Value & value ,rapidjson::Document & doc_ , rapidjson::Value * up_where_list_ = NULL ):root( &( value ) ) ,
																																		doc( &doc_ ),
																																		project_lists(NULL),
																																		from_list		 (NULL),
																																		where_list	 (NULL),
																																		up_where_list (up_where_list_),
																																		group_list	(NULL),
																																		having_list (NULL),
																																		order_list	(NULL),
																																		hints_list	(NULL),
																																		sub_links_count ( 0 )
{
select_prepared();
}

void resolve_from_list_help( rapidjson::Value  * v,int level = 0 ){
	        //rapidjson_log(v); cout<<"__________________________"<<endl;
	
					int is_alias = 0;
					int is_obvious_join = 0;

  				if( (*v)["tag"].GetInt() == T_ALIAS )is_alias = 1;
  				if( check_if_obvious_join((*v)["tag"].GetInt()) )is_obvious_join = 1;
  				
  				string table_name;  // ����
  				string alias_name;  // �����
  				
  				// ��ʽ�� ����д��
  				if(is_obvious_join){
  				   
  			  		for (int i = 1; i < 4; i++){ // ʹ�� SizeType ������ size_t
  			  		 if(i <3 )resolve_from_list( (&((*v)["children"][i])) , 0 );
  			  			if(3 == i){
  			  				join_conditions.emplace_back(&((*v)["children"][i]["JOIN_CONDITION"]));
  			  				resolve_where_list(  &((*v)["children"][i]["JOIN_CONDITION"]),v );
  			  			}
  			  	}
  			  }
  				
  				// ��ʽ�� ����д��
  				if(is_alias){
  				 table_name=string((*v)["RELATION"]["str_value_"].GetString()); 	// ����				
  				 alias_name=string((*v)["RELATION_ALIAS"]["str_value_"].GetString()); 					
            
            
            //2. �Ӳ�ѯ��������Ӳ�ѯ�ޱ���
            if( (*v)["RELATION"]["tag"].GetInt() == T_SUB_SELECT ){
            			(*v)["RELATION"].AddMember("DEEP_LEVEL", level+1, doc->GetAllocator());
            			//sub_querys.emplace_back(&(v) );
  								// �ݹ��Ӳ�ѯ
  								if( (*v)["SUB_SELECT"].HasMember("FROM_CLAUSE") )resolve_from_list( &((*v)["SUB_SELECT"]["FROM_CLAUSE"]), level+1 );
            	}
      		}	
							 // ���������ı�
  			 			 tables.emplace_back( TableItem(table_name,alias_name) );
}

//���� from_list
void resolve_from_list( rapidjson::Value  * fromlist,int level = 0 ){
  // ����ͨ�� fromlist �ĸ����ж��Ƿ����
   if( fromlist->HasMember("children")  &&  (*fromlist)["children"].GetArray().Size() != 1 )is_join = 1 ;else is_join = 0;
   //���� from_list �����ݱ����� table_names, ���Ӳ�ѯ ���� sub_querys
  	if( fromlist->IsArray() ) {
  		for (auto& v : ( (*fromlist).GetArray() )  ){  				
  				resolve_from_list( &(v),level);
         }//end for
    }
    else if(NULL != fromlist)resolve_from_list_help(fromlist,level);
         
} // end 




// ���� where_list
void resolve_where_list( rapidjson::Value  * where_list,rapidjson::Value  * where_list_father ){
	
	resolve_single_condition( where_list ,where_list_father );
   	
}


// �ж� 1 , ʶ���������
// �ж� 2 , ʶ��������  exists, in, not exists , not in
// �ж� 3 ��ʶ����ͨ����
// �ж� 4�� ʶ�������� 5 > 3
 void resolve_single_condition( rapidjson::Value  * v ,rapidjson::Value  *  father){
 			int tmp[2] = {0};
 	    int i = 0;
			
			//1. ������,�﷨�����׶��Ѿ������ǩ��
			if ( v->HasMember("SUB_LINK_TYPE") ) {
					 // ��¼ �����ӵĸ���
					 ++sub_links_count;
					// �ݹ鴦�� ������
					 sub_links.emplace_back( (*v)["SUB_LINK_BODY"], *doc , where_list );
  				 return;
			} 
			
			  //2 ������������
			if ( v->HasMember("children") && (*v)["children"].GetArray().Size()== 1 ){
				for (auto& vv : (*v)["children"].GetArray()) 
					{
						tmp[i++] = vv["tag"].GetInt() ;
						cout<<vv["tag"].GetInt()<<endl;
					}
						cout<< " Resolve_One_Oper_condition " <<endl;
			  				 		  				 
				// �ǳ�������
				if( check_if_const_value(tmp[0])   ){
						swap_father( father,v, T_CONST_CONDITION, "CONST_CONDITION"  );
						const_conditions.emplace_back( &(*father)["CONST_CONDITION"]  );
						return ;
				}
				// ��ĳ���ֶε����
				if	( check_if_name_filed(tmp[0])  ){
					 swap_father( father,v, T_NORMAL_SINGLE_CONDITION, "NORMAL_SINGLE_CONDITION"  );
				   nomal_single_conditions.emplace_back( &(*father)["NORMAL_SINGLE_CONDITION"]  );
				   return ;
				}		  
			}
			
			// 3. ���� ���������������ǩ
			if ( v->HasMember("children") && (*v)["children"].GetArray().Size()== 2 ){
					for (auto& vv : (*v)["children"].GetArray()) 
						{
							tmp[i++] = vv["tag"].GetInt() ;
							cout<<vv["tag"].GetInt()<<endl;
						}
				 	cout<< " Resolve_Two_Oper_condition " <<endl;

				 	// ��������
				 	if( check_if_join_condition(tmp[0]) && check_if_join_condition(tmp[1]) && (*v)["tag"].GetInt() == T_OP_EQ ){
						 cout<< "�������� " <<endl;
				 		 swap_father( father,v, T_JOIN_CONDITION, "JOIN_CONDITION"  );
				 		 join_conditions.emplace_back( &(*father)["JOIN_CONDITION"] );
				 		 return ;
				 	}
				 
				 // �������ǳ�������
				 if(  ( check_if_const_value(tmp[0]) && check_if_const_value(tmp[1]) ) ){
				 				
				 				//v->AddMember("is_const_condition", 1, doc->GetAllocator());
				 				//const_conditions.emplace_back(v);
				 				
				 		    swap_father( father,v, T_CONST_CONDITION, "CONST_CONDITION"   );
				 				const_conditions.emplace_back( &(*father)["CONST_CONDITION"]  );
				 				return ;
				 }
				 
				 // ����һ�������ǳ������������һ���������ֶε����
				 if(   check_if_const_value(tmp[0]) && check_if_name_filed(tmp[1]) ) 
				 	{
				 		 swap_father( father,v, T_NORMAL_DOUBLE_CONDITION, "NORMAL_DOUBLE_CONDITION"  );
				 		 //resolve_where_list( &((*father)["NORMAL_DOUBLE_CONDITION"]["children"][1]), &((*father)["NORMAL_DOUBLE_CONDITION"]["children"][1]) );
						 nomal_double_conditions.emplace_back( &(*father)["NORMAL_DOUBLE_CONDITION"]  );
						 return ;
				 		
				 	}
				 	// ����һ�������ǳ���,��һ���������ֶε����
				 if	( check_if_const_value(tmp[1]) && check_if_name_filed(tmp[0])  ){
				 
				 	 swap_father( father,v, T_NORMAL_DOUBLE_CONDITION, "NORMAL_DOUBLE_CONDITION"  );
				 	 //resolve_where_list( &((*father)["NORMAL_DOUBLE_CONDITION"]["children"][0]), &((*father)["NORMAL_DOUBLE_CONDITION"]["children"][0]) );
					 nomal_double_conditions.emplace_back( &(*father)["NORMAL_DOUBLE_CONDITION"]  );
					 return ;
				 }
				 		
				 		
				 // �߼���ϵ �������������� --> �ɼ���ϸ��
				 if(  check_if_logic_predication( (*v)["tag"].GetInt() ) )
				 	{
				 		 	  swap_father( father,v, T_COMPLEX_DOUBLE_CONDITION, "COMPLEX_DOUBLE_CONDITION"  );
				 				complex_double_conditions.emplace_back( &( (*father)["COMPLEX_DOUBLE_CONDITION"] ) );
				 				cout<< "�ݹ�ϸ�� " <<endl;
				 				// �ݹ�ϸ��
				 			   resolve_where_list( &((*father)["COMPLEX_DOUBLE_CONDITION"]["children"][0]), &((*father)["COMPLEX_DOUBLE_CONDITION"]["children"][0]) );
				 			 	 resolve_where_list( &((*father)["COMPLEX_DOUBLE_CONDITION"]["children"][1]), &((*father)["COMPLEX_DOUBLE_CONDITION"]["children"][1]) );

				 				return ;
				 }
				 				 
			}
			
		  // 3. ���� 3�������������ǩ
			if ( v->HasMember("children") && (*v)["children"].GetArray().Size()== 3 ){
					
					int tmp[3] = {0};
					for (auto& vv : (*v)["children"].GetArray()) 
						{
							tmp[i++] = vv["tag"].GetInt() ;
							cout<<vv["tag"].GetInt()<<endl;
						}
					if( (*v)["tag"].GetInt() == T_OP_BTW && check_if_const_value(tmp[1])  &&  check_if_const_value(tmp[2]) )
					{
						swap_father( father,v, T_BTW_CONDITION, "NORMAL_BTW_CONDITION"  );
						nomal_btw_conditions.emplace_back( (&(*father)["NORMAL_BTW_CONDITION"]) );
					}
			}
 	
 }

int select_prepared()
{
	// ����У��
	if(!root)return -1;
	//if( (*root)["tag"]!= T_STMT_LIST || (!root->HasMember("children")) || (*root)["children"].Size() == 0 )return -1;
	
		// ������ֵ
    //����Ƿ��� distinct			                                     
	  
	  	if(root->HasMember("T_DISTINCT") )is_distinct = 1 ; else is_distinct = 0;
	 //����Ƿ��� select_clause                                     
	  check_and_assign_member(project_lists,"SELECT_CLAUSE");        
	  //����Ƿ��� from_list                                         
	  check_and_assign_member(from_list,"FROM_CLAUSE");              
	  //����Ƿ��� where_list                                        
	  check_and_assign_member(where_list,"WHERE_CLAUSE");                   
	   //����Ƿ��� group_list                                       
	  check_and_assign_member(group_list,"group_by");                
	   //����Ƿ��� having_list                                      
	  check_and_assign_member(having_list,"having_list");            
	  //����Ƿ��� order_list                                        
	  check_and_assign_member(order_list,"order_by");                


    if(NULL != from_list){
    		resolve_from_list ( from_list );
    			if(NULL != where_list)resolve_where_list(where_list,where_list);
    }
}

};

#endif
