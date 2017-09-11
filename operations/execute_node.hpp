//  g++ -w -std=c++11  execute_node.cpp  -o cc.exe  
#ifndef EXEC_NODE
#define EXEC_NODE

#include <tuple>
#include <type_traits>
#include <utility>
#include <functional>

// ʹ��tuple ��Ϊ���������б�
template<size_t N>
struct Apply {
template<typename F, typename T, typename... A>
static inline auto apply(F && f, T && t, A &&... a)
-> decltype(Apply<N-1>::apply(
::std::forward<F>(f), ::std::forward<T>(t),
::std::get<N-1>(::std::forward<T>(t)), 
::std::forward<A>(a)...
))
{
return Apply<N-1>::apply(::std::forward<F>(f), 
::std::forward<T>(t),
::std::get<N-1>(::std::forward<T>(t)), 
::std::forward<A>(a)...
);
}
};

template<>
struct Apply<0> {
template<typename F, typename T, typename... A>
static inline auto apply(F && f, T &&, A &&... a)
-> decltype(::std::forward<F>(f) 
(::std::forward<A>(a)...))
{
return ::std::forward<F>(f)(::std::forward<A> 
(a)...);
}
};

template<typename F, typename T>
inline auto apply(F && f, T && t)
-> decltype(Apply< ::std::tuple_size<
typename ::std::decay<T>::type
>::value>::apply(::std::forward<F>(f), 
::std::forward<T>(t)))
{
return Apply< ::std::tuple_size<
typename ::std::decay<T>::type
>::value>::apply(::std::forward<F>(f), 
::std::forward<T>(t));
}

//��ִ�к�����װ��
template<class Funtype,typename ... Args>
struct exec_fun
{
	Funtype  f;
	std::tuple<Args...> _arguments;
	
	exec_fun(Funtype && _f,Args... args ):f(std::forward<Funtype>(_f)),_arguments(std::make_tuple(args...)){}
  exec_fun(){}
  
	typedef typename std::result_of<Funtype(Args...)>::type  ret_type;

   void swap(exec_fun< Funtype,Args...> & node)
  {
  	f = std::move(node.f);
  	_arguments = 	std::move(node._arguments);
  }

	//inline auto exe() -> decltype(f(std::forward<Arg1>(arg1),std::forward<Arg2>(arg2),std::forward<Arg3>(arg3),std::forward<Arg4>(arg4)))
	inline ret_type exe() 
	{
		//return f(arg1,arg2,arg3,arg4 );
		return apply( f, _arguments);
	}
	
};


#define OPERATION_START 1
#define OPERATION_END   2
/*  
��������ִ�нڵ�

pre_node1    							  <--(����)    current_node    <--(����)    out_node
   |																																			|
   |brother�����������Ľڵ��ǲ��еĹ�ϵ��																|brother�����������Ľڵ��ǲ��еĹ�ϵ��
   |																																			|
pre_node2																															out_node2
   |																																			|
   |brother																																|brother
   |																																			|
pre_node3      																												out_node3
	 |																																			|
	NULL																																	 NULL
*/

// ����ִ�мƻ�
// ����ڵ�
// pre_type     ǰ�̽ڵ�����
// brother_type �ֵܽڵ�����
// OpperType    �����ڵ�����
template<class pre_type,class brother_type,class OpperType>
struct exec_node_type
{

	typedef pre_type  						pre_type_;
	//typedef out_type  					out_type_;
	typedef brother_type  				brother_type_;
	typedef OpperType  						OpperType_;
	
	 //ִ�нڵ㷵������
	typedef typename OpperType::ret_type  ret_type; 
  
  //ǰ�̽ڵ�����
	typedef exec_node_type<typename pre_type_::pre_type_ , typename pre_type_::brother_type_,
									  typename pre_type_::OpperType_
								   > input_node_type;		 	
	
	//��̽ڵ�����							 	
	//typedef exec_node<typename out_type_::FunType_, typename out_type_::Arg1_ , typename out_type_::Arg2_ , 
	//																						 typename out_type_::Arg3_ , typename out_type_::Arg4_ , 
	//							 typename out_type_::pre_type_,typename out_type_::out_type , typename out_type_::brother_type_  > output_node_type;
								 	
	
	int    									operation_type;		//��������
	short  									is_start_end;			//�Ƿ��ǿ�ʼ�ڵ� 1 ��ʼ   2 ����
	char   									is_done;					//�Ƿ����  0 δ����   1  ����
	OpperType 							exec_node;   			//ִ�нڵ�
	
	input_node_type				* input_node;  			//ǰ�������ڵ�
	brother_type          * brother;  				//ǰ�������ֵܽڵ� 
	ret_type 								ret;							//ִ�н����
 
  exec_node_type(int _operation_type ):operation_type(_operation_type),is_start_end(OPERATION_START),is_done(0),input_node( NULL ),brother( NULL ){}
  
  //����ִ�к���
  void set_exec(OpperType & _exec_node)
  {
  	exec_node.swap(_exec_node);
  }
  
   //����ǰ�̽ڵ�
  void set_input_node(input_node_type & _input_node )
  {
  	input_node = &_input_node;
  }
  
   //�����ֵܽڵ�
  void set_brother_node(brother_type & _brohter_node )
  {
  	brother = &_brohter_node;
  }
  
	inline int check( int * pre_brother)
	{
	 return 0;  //�˳�����
	}
	
  //�ݹ���ǰ�̽ڵ�
	template<class T>
	inline int check( T*  pre_brother)
	{
	 if(pre_brother->brother == NULL)return 0;  //�˳�����
	 typename T::brother_type_ * brother_tmp = pre_brother->brother;
	 int ret = check_isdone(brother_tmp);
	 if( ret == 1 )return 1;       //�жϷ���
	 if( ret == 0 )return 0;       //�˳�����
	 return check(brother_tmp) ;
	
	}
	  //�ݹ���ǰ�̽ڵ�
	template<class T>
	inline int check_isdone( T*  brother_tmp)
	{
		if(brother_tmp->is_done!= 1)return 1;       //�жϷ���
	  else return 2;
  }
  inline int check_isdone( int*  brother_tmp)
	{
		return 0;
  }

  //ִ�б��ڵ�
	inline int try_execute()
	{
		//1 ���е�ǰ�̽ڵ㶼����
		  int first_check = 1;
		  while( first_check )
		  {
		 		  first_check = 0;
					if(first_check && input_node != NULL && input_node->is_done != 1)
					{
						 typename input_node_type::brother_type_ * brother_tmp =input_node->brother;
				   	if ( check(brother_tmp)  )
									{
										//�����ܿ�ʼ
										first_check = 1;
										// ��������
										continue;
									}
					}
			
		  }
	  	
		
		//2 ִ�б��ڵ�����
		ret = exec_node.exe();
		
		//3 ���еĺ�̽ڵ���Կ�ʼ
		is_done = 1;
		
		//4 �������еĺ�̽ڵ�Ͷ�뵽�������

	}
	
}__attribute__ ((packed, aligned (64)));

//��ִ�нڵ�
template <typename ... Args>
struct exec_fun< int,Args...>
{
	typedef  int  ret_type;
	exec_fun(){}
  void swap()
  {
  }
	inline int exe()
	{
		return 0;
	}
	
};

// ��ִ�и����ڵ�
template<class OpperType>
struct exec_node_type<int,int,OpperType>
{
	typedef int             	pre_type_;
	//typedef int  						out_type_;
	typedef int             	brother_type_;
	typedef int  							OpperType_;
	typedef int  					  	ret_type; 
	typedef int	            	input_node_type;
	//typedef int 			output_node_type;
	exec_fun< int,OpperType>    exec_node;    //ִ�нڵ�
	short  is_start_end;															//�Ƿ��ǿ�ʼ�ڵ� 1 ��ʼ   2 ����
	char   is_done;			
	input_node_type       *   input_node;  			      //ǰ�������ڵ�
	brother_type_			    *   brother;  				      //ǰ�������ֵܽڵ� 
	//int 									ret;				   			      //ִ�н����
	exec_node_type():input_node(NULL),brother(NULL),is_start_end(OPERATION_END),is_done(1){}
	inline int try_execute()
	{
		return 0;
  }
};


#endif 