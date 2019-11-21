#ifndef ITERATOR_H
#define ITERATOR_H
namespace CCSTL{
	struct input_iterator_tag {};        // 只读, 单向移动, 只支持++
	struct ouput_iterator_tag {};        // 只写, 单向移动, 只支持++
	// 可读可写, 单向移动, 只支持++
	struct forward_iterator_tag: public input_iterator_tag {};      
	// 双向移动, 只支持++和--
	struct bidirectional_iterator_tag: public forward_iterator_tag {}; 
	// 支持++和--, p+n, p-n, p[n], p1-p2, p1<p2
	struct random_access_iterator_tag: public bidirectional_iterator_tag {}; 
}
#endif