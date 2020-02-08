#ifndef RWLOCK_H
#define RWLOCK_H
#include <pthread.h>
// From APUE chapter 11.6.4

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct job {
    struct job *j_next;
    struct job *j_prev;
    pthread_t  j_id;   /* tells which thread handles this job */
	int chunk_size;
	int offset;
	char *addr;
	struct result_queue *r_queue;
};

struct queue {
    struct job *q_head;
    struct job *q_tail;
    pthread_rwlock_t q_lock;
};

struct result_queue {
	struct result *q_head;
    struct result *q_tail;
};

struct result {
	int count;
	char character;
	struct result *next;
	struct result *prev;
};

/*
 * Initialize a queue.
 */
int
queue_init(struct queue *qp)
{
	int err;

	qp->q_head = NULL;
	qp->q_tail = NULL;
	err = pthread_rwlock_init(&qp->q_lock, NULL);
	if (err != 0)
		return(err);
	/* ... continue initialization ... */
	return(0);
}

/*
 * Insert a job at the head of the queue.
 */
void
job_insert(struct queue *qp, struct job *jp)
{
	pthread_rwlock_wrlock(&qp->q_lock);
	jp->j_next = qp->q_head;
	jp->j_prev = NULL;
	struct result_queue r_queue;
	r_queue.q_head = NULL;
	r_queue.q_tail = NULL;
	jp->r_queue = &r_queue;
	if (qp->q_head != NULL)
		qp->q_head->j_prev = jp;
	else
		qp->q_tail = jp;	/* list was empty */
	qp->q_head = jp;
	pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Append a job on the tail of the queue.
 */
void
job_append(struct queue *qp, struct job *jp)
{
	pthread_rwlock_wrlock(&qp->q_lock);
	jp->j_next = NULL;
	jp->j_prev = qp->q_tail;
	struct result_queue r_queue;
	r_queue.q_head = NULL;
	r_queue.q_tail = NULL;
	jp->r_queue = &r_queue;
	if (qp->q_tail != NULL)
		qp->q_tail->j_next = jp;
	else
		qp->q_head = jp;	/* list was empty */
	qp->q_tail = jp;
	pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Remove the given job from a queue.
 */
void
job_remove(struct queue *qp, struct job *jp)
{
	pthread_rwlock_wrlock(&qp->q_lock);
	if (jp == qp->q_head) {
		qp->q_head = jp->j_next;
		if (qp->q_tail == jp)
			qp->q_tail = NULL;
		else
			jp->j_next->j_prev = jp->j_prev;
	} else if (jp == qp->q_tail) {
		qp->q_tail = jp->j_prev;
		jp->j_prev->j_next = jp->j_next;
	} else {
		jp->j_prev->j_next = jp->j_next;
		jp->j_next->j_prev = jp->j_prev;
	}
	pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Find a job for the given thread ID.
 */
struct job *
job_find(struct queue *qp, pthread_t id)
{
	struct job *jp;

	if (pthread_rwlock_rdlock(&qp->q_lock) != 0)
		return(NULL);

	for (jp = qp->q_head; jp != NULL; jp = jp->j_next)
		if (pthread_equal(jp->j_id, id))
			break;

	pthread_rwlock_unlock(&qp->q_lock);
	return(jp);
}

int
check_job_done(struct queue *qp)
{
	struct job *jp;

	if (pthread_rwlock_rdlock(&qp->q_lock) != 0)
		return -1;

	for (jp = qp->q_head; jp != NULL; jp = jp->j_next) {
		if (jp->j_id != NULL) {
			pthread_rwlock_unlock(&qp->q_lock);
			return 0;
		}
	}

	pthread_rwlock_unlock(&qp->q_lock);
	return 1;
}

void
append_result(struct result_queue *qp, int count, char character)
{
    struct result *new_result = malloc(sizeof *new_result);
    if (new_result == NULL)
        handle_error("malloc");
    new_result->character = character;
    new_result->count = count;
	new_result->next = NULL;
    if (qp->q_head == NULL) {
		new_result->prev = NULL;
        qp->q_head = new_result;
        qp->q_tail = new_result;
    } else {
        new_result->prev = qp->q_tail;
        qp->q_tail->next = new_result;
        qp->q_tail = new_result;
    }
}

#endif
