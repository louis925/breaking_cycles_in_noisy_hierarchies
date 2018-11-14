#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>
#include <vector>
#include <list>
#include <stdio.h>

#include "queue.h"

class graph {
	public:
		graph() {}

		graph(uint64_t n, uint64_t m) {reset(n, m);}
		graph(const graph & g);

		void reset(uint64_t n, uint64_t m);
		void copy(const graph & g);

		struct edge;

		TAILQ_HEAD(edgelist, edge);

		struct node {
			uint64_t id;
			edgelist out, in;
			uint64_t ind, outd;
		};

		struct edge {
			node *parent, *child;
			uint64_t id;
			bool bound;
			TAILQ_ENTRY(edge) from, to;
		};

		void bind(uint64_t k, uint64_t n, uint64_t m) {bind(getedge(k), getnode(n), getnode(m));}

		void
		bind(edge *e, node *n, node *m)
		{
			TAILQ_INSERT_TAIL(&n->out, e, from);
			TAILQ_INSERT_TAIL(&m->in, e, to);
			n->outd++;
			m->ind++;
			e->bound = true;
			e->parent = n;
			e->child = m;
		}

		void
		unbind(edge *e)
		{
			TAILQ_REMOVE(&e->parent->out, e, from);
			TAILQ_REMOVE(&e->child->in, e, to);
			e->parent->outd--;
			e->child->ind--;
			e->bound = false;
		}

		void
		unbind(node *n)
		{
			while (!TAILQ_EMPTY(&n->out)) unbind(TAILQ_FIRST(&n->out));
			while (!TAILQ_EMPTY(&n->in)) unbind(TAILQ_FIRST(&n->in));
		}

		node * getnode(uint64_t i) {return &m_nodes[i];} 
		edge * getedge(uint64_t i) {return &m_edges[i];} 

	protected:
		const graph & operator = (const graph & ) {return *this;}
		typedef std::vector<node> nodevector;
		typedef std::vector<edge> edgevector;

		nodevector m_nodes;
		edgevector m_edges;
};

class agony
{
	public:

		struct node {
			uint64_t id;
			uint64_t label;

			uint64_t rank;
			uint64_t newrank;
			uint64_t diff;

			node *parent;
			uint64_t paredge;

			uint64_t count;

			TAILQ_ENTRY(node) entries, active;
		};

		TAILQ_HEAD(nodehead, node);

		struct edge {
			bool eulerian;
			uint64_t id;
			uint64_t slack;

			TAILQ_ENTRY(edge) entries;
		};

		TAILQ_HEAD(edgehead, edge);

		uint64_t size() const {return m_nodes.size();}
		node * getnode(uint64_t i) {return &m_nodes[i];} 
		edge * getedge(uint64_t i) {return &m_edges[i];} 

		void cycledfs();

		void initagony();
		void initrank();

		void read(FILE *f);
		void writeagony(FILE *f);

		void relief(uint64_t edge);

		void minagony();

		uint64_t primal() const {return m_primal;}
		uint64_t dual() const {return m_dual;}

		uint64_t cost();


	protected:
		uint64_t slack(node *v, node *u) const {return u->rank > v->rank + 1 ? u->rank - v->rank - 1 : 0;}
		uint64_t newslack(node *v, node *u) const {return u->newrank > v->newrank + 1 ? u->newrank - v->newrank - 1 : 0;}

		uint64_t slack(uint64_t eid) {return slack(from(eid), to(eid));}

		void deleteslack(uint64_t eid);
		void addslack(uint64_t eid);
		
		typedef std::vector<node> nodevector;
		typedef std::vector<edge> edgevector;
		typedef std::vector<nodehead> nodequeue;
		typedef std::vector<edgehead> edgequeue;

		typedef std::list<node *> nodelist;

		void updaterelief(nodelist & nl);
		void resetrelief(nodelist & nl);
		void shiftrank(nodelist & nl, uint64_t s);
		void extractcycle(uint64_t edge);

		node *from(uint64_t eid) {return getnode(m_graph.getedge(eid)->parent->id);}
		node *to(uint64_t eid) {return getnode(m_graph.getedge(eid)->child->id);}


		nodevector m_nodes;
		edgevector m_edges;

		graph m_graph;
		graph m_dag, m_euler;

		edgequeue m_slacks;
		int64_t m_curslack;

		uint64_t m_dual;
		uint64_t m_primal;
		//uint64_t m_minid;
};

#endif
