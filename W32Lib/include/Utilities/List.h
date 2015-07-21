#pragma once

namespace W32Lib
{
	template <class Type>
	class List
	{
		typedef struct _NODE
		{
			_NODE *prev;
			Type *item;
			_NODE *next;

			_NODE() : prev (0), item(0), next(0) {}
			_NODE(_NODE *prev_node, Type *new_item) : prev (prev_node), item(new_item), next(0) {}
		} NODE;

		private:

			unsigned long count;
			NODE *curr_node;
			NODE *first_node;
			NODE *last_node;

		public:

			List(void) : count(0), curr_node(0), first_node(0), last_node(0) {}
			~List(void) {Clear();}

			// ��������� ������� � ����� ������
			bool Add(Type *item);

			// ������� ������� �� ������ �� ���������� ����������� ������ <index> 
			void Remove(unsigned long index);
			
			// ���������� ��������� �������
			Type *GetItem(unsigned long index);

			// ���������� ������ ������� ������
			Type *GetFirstItem();

			// ���������� ��������� ������� ������
			Type *GetNextItem();
			
			// ���������� ���-�� ��������� � ������
			unsigned long GetCount() {return count;}

			// ������� ��� �������� �� ������
			void Clear();
	};

	template<class Type>	//*
	bool List<Type>::Add(Type *item)	
	{
		try
		{
			NODE *new_node = new NODE(last_node, item);
			if (count) {last_node->next = new_node; last_node = new_node;}
			else first_node = last_node = new_node;
			++count;
			return true;
		}
		catch(std::bad_alloc)
		{
			return false;
		}
	}

	template <class Type>
	void List<Type>::Remove(unsigned long index)
	{
		if (index < count)
		{
			NODE *node = first_node;
			for (unsigned int i = 0; i < index; ++i)
				node = node->next;

			if (node->next && node->prev)
			{
				(node->prev)->next = node->next;
				(node->next)->prev = node->prev;
			}
			if (node->next && !node->prev) // node �������� ������
			{
				(node->next)->prev = 0;
				first_node = node->next;
			}
			if (node->prev && !node->next) // node �������� ���������
			{
				(node->prev)->next = 0;
				last_node = node->prev;
			}
			if (!node->next && !node->prev) // ������ �������� ������ ���� node
			{
				first_node = last_node = 0;
			}

			--count;
			delete node;
		}
	}

	template <class Type>	//*
	Type *List<Type>::GetItem(unsigned long index)
	{
		if (index < count)
		{
			NODE *node = first_node;
			for (unsigned int i = 0; i < index; ++i)
				node = node->next;
			return node->item;
		}
		else return 0;
	}

	template <class Type>	//*
	Type *List<Type>::GetFirstItem()
	{
		if (first_node) return (curr_node = first_node)->item;
		else return 0;
	}

	template <class Type>	//*
	Type *List<Type>::GetNextItem()
	{
		if (curr_node && curr_node->next) return (curr_node = curr_node->next)->item;
		else return 0;
	}

	template <class Type>	//*
	void List<Type>::Clear()
	{
		while(count)
		{
			if (last_node->prev)
			{
				last_node = last_node->prev;
				delete last_node->next;
			}
			else delete last_node;
			--count;
		}
		first_node = last_node = curr_node = 0;
	}

}