#include <iostream>
#include <stdexcept>
#include <exception> 
#include <string>	

//--------исключени€----------
template<class Key>
class not_found_exception : public std::exception
{
public:
	virtual const Key& get_key() const noexcept = 0;
};

template<class Key>
class Output_exception : public not_found_exception<Key>
{
private:
	const Key key;
	std::string error = "my error ";

public:
	Output_exception(const Key& key) : key(key) {}
	virtual const Key& get_key() const noexcept override { return key; }
	const char* what() const noexcept override { return error.c_str(); }
};



//--------------MAP-----------------
template<class Key, class Value>
class dictionary
{
public:
	virtual ~dictionary() = default;
	virtual const Value& get(const Key& key) const = 0;
	virtual void set(const Key& key, const Value& value) = 0;
	virtual bool is_set(const Key& key) const = 0;
};

template<class Key, class Value>
class my_map :dictionary <Key, Value> {
private:
	template <class Key, class Value>
	class Cell;

public:
	my_map() {};
	my_map(const Key& key, const Value& value);
	~my_map();
	const Value& get(const Key& key) const;
	bool is_set(const Key& key) const;
	void set(const Key& key, const Value& value);
	bool erase(const Key& key);
	size_t size() const;
	void clear();
	void print();

private:
	template <class Key, class Value>
	class Cell {
	public:
		Cell* cell_left;		//левое ветвление
		Cell* cell_right;		//правое ветвление
		Key key;
		Value value;
		Cell(const Key& key = Key(), const Value& value = Value(),
			Cell* cell_left = nullptr, Cell* cell_right = nullptr) {
			this->key = key;
			this->value = value;
			this->cell_left = cell_left;
			this->cell_right = cell_right;
		}
	};
	Cell<Key, Value>* cell_head = nullptr;	//указатель на вершину/первый элемент
	size_t lenght = 0;
};

template<class Key, class Value>
my_map<Key, Value>::my_map(const Key& key, const Value& value)
{
	cell_head = new Cell<Key, Value>(key, value);
	lenght++;

}

template<class Key, class Value>
my_map<Key, Value>::~my_map()
{
	this->clear();
}

template<class Key, class Value>
const Value& my_map<Key, Value>::get(const Key& key) const
{
	Cell<Key, Value>* cell_now = cell_head;
	while (1)									//ищем совпадение
	{
		if (key == cell_now->key)
		{
			return cell_now->value;
		}
		else if (key < cell_now->key)			//идЄм влево
		{
			if (cell_now->cell_left != nullptr)	//если зан€то
			{
				cell_now = cell_now->cell_left;	//идЄм на новую итерацую
				continue;
			}
			else								//еслли свобободно, совпадений не найдено
			{									//бросаем исключение, лучше конечно возвращать bool
				throw Output_exception<Key>(key);
			}
		}
		else									 //идЄм вправо
		{
			if (cell_now->cell_right != nullptr) //если зан€то
			{
				cell_now = cell_now->cell_right; //идЄм на новую итерацую
				continue;
			}
			else								 //еслли свобободно, совпадений не найдено
			{									 //бросаем исключение, лучше конечно возвращать bool
				throw Output_exception<Key>(key);
			}
		}
	}
}

template<class Key, class Value>
bool my_map<Key, Value>::is_set(const Key& key) const
{
	return false;
}

template<class Key, class Value>
void my_map<Key, Value>::set(const Key& key, const Value& value)
{
	if (cell_head == nullptr)
	{
		cell_head = new Cell<Key, Value>(key, value);
		lenght++;
	}
	else
	{
		Cell<Key, Value>* cell_now = cell_head;
		while (1)									//ищем свободное место
		{
			if (key == cell_now->key)				//с таким ключЄм уже существует,
			{									    //бросаем исключение, лучше конечно возвращать bool
				throw Output_exception<Key>(key);
			}
			else if (key < cell_now->key)			//идЄм влево
			{
				if (cell_now->cell_left != nullptr)	//если зан€то
				{
					cell_now = cell_now->cell_left;	//идЄм на новую итерацую
					continue;
				}
				else								//еслли свобободно, записываем значение
				{
					cell_now->cell_left = new Cell<Key, Value>(key, value);
					lenght++;
					return;
				}
			}
			else									 //идЄм вправо
			{
				if (cell_now->cell_right != nullptr) //если зан€то
				{
					cell_now = cell_now->cell_right; //идЄм на новую итерацую
					continue;
				}
				else								 //еслли свобободно, записываем значение
				{
					cell_now->cell_right = new Cell<Key, Value>(key, value);
					lenght++;
					return;
				}
			}
		}
	}
}

template<class Key, class Value>
bool my_map<Key, Value>::erase(const Key& key)
{
	if (lenght == 1)
	{
		delete cell_head;
		cell_head = nullptr;
		lenght--;
		return true;
	}
	if (lenght == 0)
		return false;

	Cell<Key, Value>* previos_cell = nullptr;
	Cell<Key, Value>* now_cell = cell_head;
	bool flag_turn_right;
	while (now_cell->key != key)						 //ищем нужную €чейку
	{
		previos_cell = now_cell;
		if (key > now_cell->key)						 //идЄм вправо
		{
			if (now_cell->cell_right == nullptr)			 //если кончилась цепочка, такого
				return false;							 //элемента нет
			now_cell = now_cell->cell_right;
			flag_turn_right = true;						 //запоминаем куда повернули последний раз
		}
		else											 //идЄм влево 
		{
			if (now_cell->cell_left == nullptr)			 //если кончилась цепочка, такого
				return false;							 //элемента нет
			now_cell = now_cell->cell_left;
			flag_turn_right = false;					 //запоминаем куда повернули последний раз
		}
	}
	if (flag_turn_right)								 //если повернули направо
	{
		previos_cell->cell_right = now_cell->cell_right; //вписываем правый хвост в праввый адрес предыдешей €чейки
		if (now_cell->cell_left != nullptr)				 //если есть левое ветвление от удал€емой €чейки
		{
			Cell<Key, Value>* cell_temp = now_cell->cell_right;
			while (cell_temp->cell_left != nullptr)		 //ищем свободный левый адрес в правом хвосте
			{											 //тк левый хвост по умолчанию самый малый, всегда  
				cell_temp = cell_temp->cell_left;		 //поворачиваем налево
			}
			cell_temp->cell_left = now_cell->cell_left;
		}
		delete now_cell;
		lenght--;
		return true;
	}
	else												 //если повернули налево
	{
		previos_cell->cell_left = now_cell->cell_right; //вписываем правый хвост в левый адрес предыдешей €чейки
		if (now_cell->cell_left != nullptr)				 //если есть левое ветвление от удал€емой €чейки
		{
			Cell<Key, Value>* cell_temp = now_cell->cell_right;
			while (cell_temp->cell_left != nullptr)		 //ищем свободный левый адрес в правом хвосте
			{											 //тк левый хвост по умолчанию самый малый, всегда  
				cell_temp = cell_temp->cell_left;		 //поворачиваем налево
			}
			cell_temp->cell_left = now_cell->cell_left;
		}
		delete now_cell;
		lenght--;
		return true;
	}

}


template<class Key, class Value>
size_t my_map<Key, Value>::size() const
{
	return lenght;
}

template<class Key, class Value>
void my_map<Key, Value>::clear()
{
	Cell<Key, Value>* go_right = nullptr;
	Cell<Key, Value>* go_left = nullptr;
	if (cell_head != nullptr)
	{
		go_right = cell_head->cell_right;
		go_left = cell_head->cell_left;
		delete cell_head;
		cell_head = nullptr;
		lenght--;
	}
	if (go_left != nullptr)			//смещаемс€ влево до упора
	{
		cell_head = go_left;
		this->clear();
	}
	if (go_right != nullptr)			//смещаемс€ вправо,
	{
		cell_head = go_right;
		this->clear();
	}
}

template<class Key, class Value>
void my_map<Key, Value>::print()
{
	Cell<Key, Value>* go_right = nullptr;
	Cell<Key, Value>* go_left = nullptr;
	if (cell_head != nullptr)
	{
		go_right = cell_head->cell_right;
		go_left = cell_head->cell_left;
		std::cout << cell_head->key << "\t" << cell_head->value << std::endl;
	}
	if (go_left != nullptr)			//смещаемс€ влево до упора
	{
		cell_head = go_left;
		this->print();
	}
	if (go_right != nullptr)			//смещаемс€ вправо,
	{
		cell_head = go_right;
		this->print();
	}
}




int main() {
	my_map<int, std::string> map;
	map.set(8, "3");
	map.set(1, "5");
	map.set(3, "7");
	map.print();
}

