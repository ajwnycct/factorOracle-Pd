/*
 
 factorOracle, a Pure Data external
 Adam James Wilson
 awilson@citytech.cuny.edu
 
 LICENSE:
 
 This software is copyrighted by Adam James Wilson and others. The following terms (the "Standard Improved BSD License") apply:
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 */




#include "m_pd.h"
#include "time.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>




static t_class *proxy_class = NULL;
static t_class *fopenpanel_class = NULL;
static t_class *factorOracle_class = NULL;




typedef struct _proxy {
    t_pd l_pd;
    void *factorOracle;
} t_proxy;




typedef struct _fopenpanel
{
    t_pd l_pd;
    t_symbol *x_s;
    void *factorOracle;
} t_fopenpanel;




typedef struct _state
{
    long suffixLink;
    long numberOfTransitionElements;
    long transitionElement;
    long *transitionEndStates;
} t_state;




typedef struct _factorOracle
{
    t_object x_obj;
    
    t_canvas *canvas;
    t_symbol *canvas_dir;
    t_symbol *x_s;
    t_proxy pxy;
    t_fopenpanel fopenpanel;
    
    t_outlet *m_outlet1;
    t_outlet *m_outlet2;
    t_outlet *m_outlet3;
    t_outlet *m_outlet4;
    t_outlet *m_outlet5;
    t_outlet *m_outlet6;
    t_outlet *m_outlet10;
    
    long *alphabet;
    long alphabet_size;
    char *json;
    long json_size;
    t_state *states;
    long *input_string;
    long input_limit;
    long input_index;
    long output_state;
    long *output_string;
    long output_limit;
    long output_index;
    long default_size;
    double probability;
    long mode;
    long previousRoute;
} t_factorOracle;




static const char *MEMORY_ALLOCATION_ERROR = "Unable to allocate memory.";
static const char *EMPTY_ORACLE_ERROR = "The oracle is empty.";




void *factorOracle_new(t_symbol *s, int argc, t_atom *argv);
void factorOracle_free(t_factorOracle *x);
void factorOracle_bang(t_factorOracle *x);
void factorOracle_float(t_factorOracle *x, float transition);
void factorOracle_state(t_factorOracle *x, float state);
void factorOracle_mode(t_factorOracle *x, float mode);
void factorOracle_probability(t_factorOracle *x, float probability);
void factorOracle_clear(t_factorOracle *x);
void factorOracle_anything(t_factorOracle *x, t_symbol *s, int argc, t_atom *argv);
long factorOracle_walk(t_factorOracle *x);
void factorOracle_doread(t_factorOracle *x, t_symbol *s);
void factorOracle_alphabet(t_factorOracle *x);
void factorOracle_dowrite(t_factorOracle *x, t_symbol *s);
long memberOfTransitionElements(long transition, long k, t_factorOracle *x);
long buildOracle(long transition, t_factorOracle *x);
int getInputString(t_factorOracle *x);
void setState(t_factorOracle *x, long state_index);
void getState(t_factorOracle *x);
long getAlphabet(t_factorOracle *x);
long json(t_factorOracle *x);
long mode_0(t_factorOracle *x);
long mode_1(t_factorOracle *x);
long mode_2(t_factorOracle *x);




static void proxy_init(t_proxy *p, t_factorOracle *obj) {
    p->l_pd = proxy_class;
    p->factorOracle = (void *)obj;
}




static void *fopenpanel_new( void)
{
    t_fopenpanel *x = (t_fopenpanel *)pd_new(fopenpanel_class);
    return (x);
}




static void fopenpanel_init(t_fopenpanel *fo, t_factorOracle *obj) {
    fo->l_pd = fopenpanel_class;
    char buf[MAXPDSTRING];
    sprintf(buf, "d%lx", (t_int)fo);
    fo->x_s = gensym(buf);
    pd_bind(&fo->l_pd, fo->x_s);
    fo->factorOracle = (void *)obj;
}




static void fopenpanel_symbol(t_fopenpanel *x, t_symbol *s)
{
    t_factorOracle *fo = (t_factorOracle *)x->factorOracle;
    char *path = fo->canvas_dir->s_name;
    sys_vgui("pdtk_openpanel {%s} {%s}\n", x->x_s->s_name, path);
}




static void fopenpanel_callback(t_fopenpanel *x, t_symbol *s)
{
    t_factorOracle *fo = (t_factorOracle *)x->factorOracle;
    factorOracle_doread(fo, s);
}




static void fopenpanel_free(t_fopenpanel *x)
{
    pd_unbind(&x->l_pd, x->x_s);
}




static void fopenpanel_setup(void)
{
    fopenpanel_class = class_new(gensym("fopenpanel"),
                                 (t_newmethod)fopenpanel_new, (t_method)fopenpanel_free,
                                 sizeof(t_fopenpanel), 0, 0);
    class_addsymbol(fopenpanel_class, fopenpanel_symbol);
    class_addmethod(fopenpanel_class, (t_method)fopenpanel_callback,
                    gensym("callback"), A_SYMBOL, 0);
}




static void proxy_bang(t_proxy *x)
{
    t_factorOracle *fo = (t_factorOracle *)(x->factorOracle);
    getState(fo);
}




static void proxy_state(t_proxy *x, float state)
{
    t_factorOracle *fo = (t_factorOracle *)(x->factorOracle);
    setState(fo, (long)state);
}




static void *proxy_new(t_symbol *s, int argc, t_atom *argv) {
    t_proxy *x = (t_proxy *)pd_new(proxy_class);
    if (x) {
        post("proxy_new");
    } else {
        post("proxy_new: could not create memory for new");
    }
    return x;
}




static void proxy_free(t_proxy *p) {
    post("proxy_free");
}




static void proxy_setup(void) {
    post("proxy_setup");
    proxy_class =
    (t_class *)class_new(gensym("proxy"),
                         (t_newmethod)proxy_new,
                         (t_method)proxy_free,
                         sizeof(t_proxy),
                         0,
                         A_GIMME,
                         0);
    class_addbang(proxy_class, (t_method)proxy_bang);
    class_addmethod(proxy_class, (t_method)proxy_state, gensym("float"), A_FLOAT, 0);
}




void *factorOracle_new(t_symbol *s, int argc, t_atom *argv)
{
    t_factorOracle *x = NULL;
    
    if ((x = (t_factorOracle *)pd_new(factorOracle_class))) {

        fopenpanel_init(&x->fopenpanel, x);
        
        proxy_init(&x -> pxy, x);
        inlet_new(&x -> x_obj, &x -> pxy.l_pd, 0, 0);
        
        inlet_new(&x -> x_obj, &x->x_obj.ob_pd, &s_float, gensym("probability"));
        
        x->m_outlet10 =  outlet_new(&x->x_obj, &s_float);
        x->m_outlet6  =  outlet_new(&x->x_obj, &s_float);
        x->m_outlet5  =  outlet_new(&x->x_obj, &s_list);
        x->m_outlet4  =  outlet_new(&x->x_obj, &s_list);
        x->m_outlet3  =  outlet_new(&x->x_obj, &s_float);
        x->m_outlet2  =  outlet_new(&x->x_obj, &s_float);
        x->m_outlet1  =  outlet_new(&x->x_obj, &s_float);
        
        x->input_index = 0;
        x->output_index = 0;
        x->output_state = -1;
        x->default_size = 10000;
        
        x->mode = 0;
        x->probability = 0.75;
        x->canvas = canvas_getcurrent();
        x->canvas_dir = canvas_getcurrentdir();
        
        if (argc >= 1 && ((argv)->a_type == A_FLOAT) && (atom_getfloat(argv) > -1))
        {
            x->states = getbytes(atom_getfloat(argv+0) * sizeof(t_state));
            x->input_limit = (long)atom_getfloat(argv+0);
            x->output_string = getbytes((long)(atom_getfloat(argv+0)) * sizeof(long));
            x->output_limit = (long)atom_getfloat(argv+0);
            post("Number of states allocated for input: %ld.", (long)atom_getfloat(argv+0));
        }
        else
        {
            x->states = getbytes(x->default_size * sizeof(t_state));
            x->input_limit = x->default_size;
            x->output_string = getbytes(x->default_size * sizeof(long));
            x->output_limit = x->default_size;
            post("Argument 1 must be an integer greater than 0 specifying the number of input states. Allocating default: %ld.", x->default_size);
        }
        
        if (argc > 1)
        {
            if ((argv+1)->a_type == A_SYMBOL)
            {
                factorOracle_doread(x, atom_getsymbol(argv+1));
            }
            else
            {
                pd_error((t_object *)x, "Input file '%s' failed to load.", atom_getsymbol(argv+1)->s_name);
            }
        }
        
        if (argc > 2)
        {
            post("Ignoring extra arguments.");
        }
    }
    
    srand((unsigned)time(NULL));
    return (void *)x;
}




void factorOracle_setup(void) {
    factorOracle_class =
    (t_class *)class_new(gensym("factorOracle"),
                         (t_newmethod)factorOracle_new,
                         0,
                         sizeof(t_factorOracle),
                         CLASS_DEFAULT,
                         A_GIMME,
                         0);
    class_addbang(factorOracle_class, factorOracle_bang);
    class_addmethod(factorOracle_class, (t_method)factorOracle_mode, gensym("mode"), A_FLOAT, 0);
    class_addmethod(factorOracle_class, (t_method)factorOracle_float, gensym("float"), A_FLOAT, 0);
    class_addmethod(factorOracle_class, (t_method)factorOracle_clear, gensym("clear"), 0);
    class_addmethod(factorOracle_class, (t_method)factorOracle_probability, gensym("probability"), A_FLOAT, 0);
    class_addanything(factorOracle_class, (t_method)factorOracle_anything);
    proxy_setup();
    fopenpanel_setup();
}




void factorOracle_free(t_factorOracle *x)
{
    freebytes(x->alphabet, x->alphabet_size * sizeof(long));
    freebytes(x->input_string, x->input_index * sizeof(long));
    freebytes(x->output_string, x->input_index * sizeof(long));

    for (long i = 0; i < x->input_index; i ++)
    {
        freebytes(x->states[i].transitionEndStates, x->states[i].numberOfTransitionElements * sizeof(long));
    }
    freebytes(x->states, x->input_limit + 1);
}




long memberOfTransitionElements(long transition, long k, t_factorOracle *x)
{
    long test = -1;
    long i;
    for (i = 0; i < x->states[k].numberOfTransitionElements; i++)
    {
        if (transition == x->states[(x->states[k].transitionEndStates[i]) - 1].transitionElement)
        {
            test = i;
            break;
        }
    }
    return test;
}




long buildOracle(long transition, t_factorOracle *x)
{
    x->states[x->input_index].transitionElement = transition;
    x->states[x->input_index].transitionEndStates = getbytes(sizeof(long));
    if (x->states[x->input_index].transitionEndStates == NULL)
    {
        post("%s", MEMORY_ALLOCATION_ERROR);
        return -1;
    }
    
    x->states[x->input_index].transitionEndStates[0] = (x->input_index + 1);
    x->states[x->input_index].numberOfTransitionElements = 1;
    
    long k;
    if (x->input_index == 0)
    {
        x->states[x->input_index].suffixLink = -1;
        k = -1;
    }
    else
    {
        k = x->states[x->input_index].suffixLink;
    }
    
    while ((k != -1) && ((memberOfTransitionElements(transition, k, x)) == -1))
    {
        x->states[k].transitionEndStates = resizebytes(x->states[k].transitionEndStates, x->states[k].numberOfTransitionElements * sizeof(long), (x->states[k].numberOfTransitionElements + 1) * sizeof(long));
        if (x->states[k].transitionEndStates == NULL)
        {
            post("%s", MEMORY_ALLOCATION_ERROR);
            return -1;
        }
        
        x->states[k].transitionEndStates[x->states[k].numberOfTransitionElements] = (x->input_index + 1);
        x->states[k].numberOfTransitionElements += 1;
        k = x->states[k].suffixLink;
    }
    
    if (k == -1)
    {
        x->states[x->input_index+1].suffixLink = 0;
    }
    else
    {
        x->states[(x->input_index + 1)].suffixLink = x->states[k].transitionEndStates[memberOfTransitionElements(transition, k, x)];
    }
    x->states[(x->input_index + 1)].numberOfTransitionElements = 0;
    
    return 0;
}




void setState(t_factorOracle *x, long state_index)
{
    if (x->input_index < 1)
    {
        post(EMPTY_ORACLE_ERROR);
        return;
    }
    
    if (state_index < 0 || state_index > x->input_index)
    {
        post("State index %ld is outside of index range [0, %ld].", state_index, x->input_index);
        return;
    }
    x->output_state = state_index;
    
    outlet_float( x->m_outlet2, x->output_state);
}




void getState(t_factorOracle *x)
{
    if (x->output_state < 0)
    {
        post("Initial output state has not been selected.");
        return;
    }
    
    t_atom *es;
    t_atom *et;
    long len;
    
    if (x->output_state == x->input_index)
    {
        es = getbytes(sizeof(t_atom));
        et = getbytes(sizeof(t_atom));
        if (es == NULL || et == NULL)
        {
            post("%s", MEMORY_ALLOCATION_ERROR);
            return;
        }
        len = 1;
        SETFLOAT(es, -1);
        SETFLOAT(et, -1);
    }
    else
    {
        len = x->states[x->output_state].numberOfTransitionElements;
        es = getbytes(len * sizeof(t_atom));
        et = getbytes(len * sizeof(t_atom));
        if (es == NULL || et == NULL)
        {
            post("%s", MEMORY_ALLOCATION_ERROR);
            return;
        }
        long endState;
        for (long i = 0; i < x->states[x->output_state].numberOfTransitionElements; i++)
        {
            endState = x->states[x->output_state].transitionEndStates[i];
            SETFLOAT(es+i, endState);
            SETFLOAT(et+i, x->states[endState - 1].transitionElement);
        }
    }

    t_float input_index = x->input_index;
    outlet_float( x->m_outlet1, input_index);
    outlet_float( x->m_outlet2, x->output_state);
    outlet_float( x->m_outlet3, x->states[x->output_state].numberOfTransitionElements);
    outlet_list(x->m_outlet4, NULL, (int)len, et);
    outlet_list(x->m_outlet5, NULL, (int)len, es);
    outlet_float( x->m_outlet6, x->states[x->output_state].suffixLink);
    
    freebytes(es, sizeof(t_atom));
    freebytes(et, sizeof(t_atom));
}




void chooseTransition(t_factorOracle *x)
{
    if (x->input_index < 1)
    {
        post(EMPTY_ORACLE_ERROR);
        return;
    }
    
    long output = 0;
    if (x->output_index < x->output_limit)
    {
        switch (x->mode)
        {
            case 0:
                output = mode_0(x);
                break;
            case 1:
                output = mode_1(x);
                break;
            case 2:
                output = mode_2(x);
                break;
        }
        
        x->output_string[x->output_index] = output;
        x->output_index += 1;
    }
    else
    {
        pd_error((t_object *)x, "%s", MEMORY_ALLOCATION_ERROR);
        return;
    }
    t_float out = output;
    outlet_float(x->m_outlet10, out);
}




void addTransition(t_factorOracle *x, long transition)
{
    if (x->input_index < x->input_limit)
    {
        if (buildOracle(transition, x) != 0)
        {
            pd_error((t_object *)x, "%s", MEMORY_ALLOCATION_ERROR);
            return;
        }
        x->input_index += 1;
    }
    else
    {
        pd_error((t_object *)x, "%s", MEMORY_ALLOCATION_ERROR);
    }
}




void factorOracle_bang(t_factorOracle *x)
{
    chooseTransition(x);
}




void factorOracle_float(t_factorOracle *x, float transition)
{
    addTransition(x, (long)transition);
}




void factorOracle_mode(t_factorOracle *x, float mode)
{
    long m = (long)mode;
    if (m < 0)
    {
        x->mode = 0;
    }
    else if (m > 1)
    {
        x->mode = 1;
    }
    else
    {
        x->mode = m;
    }
}




int getInputString(t_factorOracle *x)
{
    freebytes(x->input_string, sizeof(long) * x->input_index);
    
    x->input_string = getbytes(x->input_index * sizeof(long));
    if (x->input_string == NULL) {
        post("%s", MEMORY_ALLOCATION_ERROR);
        return -1;
    }
    
    for (long i = 0; i < x->input_index; i++) {
        x->input_string[i] = x->states[i].transitionElement;
    }
    
    return 0;
}




void factorOracle_clear(t_factorOracle *x)
{
    freebytes(x->alphabet, x->alphabet_size * sizeof(long));
    freebytes(x->input_string, x->input_index * sizeof(long));
    for (long i = 0; i < x->input_index; i ++)
    {
        freebytes(x->states[i].transitionEndStates, x->states[i].numberOfTransitionElements * sizeof(long));
        x->states[i].numberOfTransitionElements = 0;
    }
    x->input_index = 0;
    x->output_index = 0;
    x->output_state = -1;
}




int compare(const void *a, const void *b)
{
    long x = *(const long *)a;
    long y = *(const long *)b;
    
    if (x < y)
    {
        return -1;
    }
    else if (x > y)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}




long getAlphabet(t_factorOracle *x)
{
    freebytes(x->alphabet, x->alphabet_size * sizeof(long));
    x->alphabet_size = 0;
    
    getInputString(x);
    
    long *tmp = getbytes(x->input_index * sizeof(long)); //get rid of clear
    if (tmp == NULL) {
        post("%s", MEMORY_ALLOCATION_ERROR);
        return -1;
    }
    
    long i;
    for (i = 0; i < x->input_index; i++) {
        tmp[i] = x->input_string[i];
    }
    
    qsort(tmp, x->input_index, sizeof(long), compare);
    
    x->alphabet = getbytes(sizeof(long));
    if (x->alphabet == NULL)
    {
        post(MEMORY_ALLOCATION_ERROR);
        return -1;
    }
    unsigned long alphabet_index = 0;
    for (i = 0; i < x->input_index - 1; i++)
    {
        if (tmp[i] != tmp[i + 1]) {
            x->alphabet[alphabet_index] = tmp[i];
            alphabet_index = alphabet_index + 1;
            x->alphabet = resizebytes(x->alphabet, alphabet_index * sizeof(long), (alphabet_index + 1) * sizeof(long));
            if (x->alphabet == NULL)
            {
                post(MEMORY_ALLOCATION_ERROR);
                return -1;
            }
        }
    }
    x->alphabet[alphabet_index] = tmp[x->input_index - 1];
    x->alphabet_size = alphabet_index + 1;
    
    freebytes(tmp, sizeof(long) * x->input_index);
    freebytes(x->input_string, sizeof(long) * x->input_index); // Conserve memory.
    
    return x->alphabet_size;
}




long json(t_factorOracle *x)
{
    unsigned long MAX_LONG_CHARS;
    if (sizeof(long) == 4)
    {
        MAX_LONG_CHARS = 12;
    }
    else
    {
        MAX_LONG_CHARS = 21;
    }

    x->json = getbytes(4 * x->input_index * MAX_LONG_CHARS * CHAR_BIT + 24 * x->input_index * CHAR_BIT - 2 * MAX_LONG_CHARS * CHAR_BIT - 2 * CHAR_BIT);
    if (x->json == NULL)
    {
        post(MEMORY_ALLOCATION_ERROR);
        return -1;
    }
    
    unsigned long tmpbuff_size = (CHAR_BIT * (MAX_LONG_CHARS * 2 + 6)), tcb = (CHAR_BIT * 2);
    char *tmpbuff = getbytes(tmpbuff_size);
    if (x->json == NULL)
    {
        post(MEMORY_ALLOCATION_ERROR);
        return -1;
    }
    
    unsigned long len, i, j, json_next_index = 0;
    long end_state, transition;
    
    len = snprintf(tmpbuff, tcb, "{\n");
    memcpy(x->json+json_next_index, tmpbuff, len);
    json_next_index += len;
    for (i = 0; i <= x->input_index; i++)
    {
        if (i > 0)
        {
            len = snprintf(tmpbuff, tcb, ",\n");
            memcpy(x->json+json_next_index, tmpbuff, len);
            json_next_index += len;
        }
        len = snprintf(tmpbuff, tmpbuff_size, "\"%ld\":[{", i);
        memcpy(x->json+json_next_index, tmpbuff, len);
        json_next_index += len;
        for (j = 0; j < x->states[i].numberOfTransitionElements; j++)
        {
            if (j > 0)
            {
                len = snprintf(tmpbuff, CHAR_BIT, ",");
                memcpy(x->json+json_next_index, tmpbuff, len);
                json_next_index += len;
            }
            end_state = x->states[i].transitionEndStates[j];
            transition = x->states[end_state - 1].transitionElement;
            len = snprintf(tmpbuff, tmpbuff_size, "\"%ld\":\"%ld\"", end_state, transition);
            memcpy(x->json+json_next_index, tmpbuff, len);
            json_next_index += len;
        }
        len = snprintf(tmpbuff, tmpbuff_size, "},\"%ld\"]", x->states[i].suffixLink);
        memcpy(x->json+json_next_index, tmpbuff, len);
        json_next_index += len;
    }
    len = snprintf(tmpbuff, tcb, "\n}");
    memcpy(x->json+json_next_index, tmpbuff, len);
    json_next_index += len;
    
    freebytes(tmpbuff, tmpbuff_size);
    return json_next_index;
}




void factorOracle_dowrite(t_factorOracle *x, t_symbol *s)
{
    if (x->input_index < 1)
    {
        pd_error((t_object *)x, "%s", EMPTY_ORACLE_ERROR);
    }
    else
    {
        t_binbuf *b = binbuf_new();
        long size = x->input_index * sizeof(t_atom);
        t_atom *argv = getbytes(size);
        for (long i = 0; i < x->input_index; i++)
        {
            SETFLOAT(&argv[i], x->states[i].transitionElement);
        }
        binbuf_add(b, (int)x->input_index, argv);
        binbuf_write(b, s->s_name, x->canvas_dir->s_name, 1);
        freebytes(b, size);
    }

}




void factorOracle_doread(t_factorOracle *x, t_symbol *s) {
    t_binbuf *b = binbuf_new();
    int ret = binbuf_read_via_canvas(b, s->s_name, x->canvas, 0);
    if (ret != 0)
    {
        pd_error((t_object *)x, "%s", MEMORY_ALLOCATION_ERROR);
        return;
    }
    
    int num_new_transitions = binbuf_getnatom(b);
    
    x->states = resizebytes(x->states, x->input_limit * sizeof(t_state), (x->input_limit + num_new_transitions + 1) * sizeof(t_state));
    if (x->states == NULL)
    {
        pd_error((t_object *)x, "%s", MEMORY_ALLOCATION_ERROR);
        return;
    }
    
    long addition_to_input_limit = 0;
    
    t_atom *q = binbuf_getvec(b);
    for (int ac = 0; ac < num_new_transitions; ac++) {
        if (q[0].a_type == A_FLOAT)
        {
            if (buildOracle((long)atom_getfloat(&q[ac]), x) == 0)
            {
                x->input_index += 1;
                addition_to_input_limit += 1;
            }
            else
            {
                pd_error((t_object *)x, "%s", MEMORY_ALLOCATION_ERROR);
                return;
            }
        }
    }
    
    x->input_limit += addition_to_input_limit;
    freebytes(b, num_new_transitions * sizeof(t_atom));
}




void factorOracle_anything(t_factorOracle *x, t_symbol *s, int argc, t_atom *argv)
{
    if (s == gensym("read"))
    {
        if (argc == 0)
        {
            fopenpanel_symbol(&x->fopenpanel, &s_);
        }
        else if (argv[0].a_type == A_SYMBOL)
        {
            factorOracle_doread(x, atom_getsymbol(argv));
        }
    }
    else if (s == gensym("write"))
    {
        if (argc == 0)
        {
            return;
        }
        else if (argv[0].a_type == A_SYMBOL)
        {
            factorOracle_dowrite(x, atom_getsymbol(argv));
        }
    }
    else
    {
        return;
    }
}




void factorOracle_probability(t_factorOracle *x, float probability)
{
    if (probability > 1.0) {
        x->probability = 1.0;
    } else if (probability < 0.0) {
        x->probability = 0;
    } else {
        x->probability = probability;
    }
}




long jumpBack(t_factorOracle *x, long stateIndex) {
    long linkIndex;
    while (x->states[stateIndex].suffixLink != 0) {
        linkIndex = x->states[stateIndex].suffixLink;
        if ((stateIndex - linkIndex) > 1) {
            return linkIndex;
        } else {
            stateIndex = linkIndex;
        }
    }
    return 0;
}




long mode_0(t_factorOracle *x) {
    return factorOracle_walk(x);
}




long mode_1(t_factorOracle *x) {
    return 0;
}




long mode_2(t_factorOracle *x) {
    return 0;
}




long factorOracle_walk(t_factorOracle *x) {
    if ((x->output_state == -1) || (x->output_state == x->input_index))
    {
        long suffixState = jumpBack(x, x->input_index);
        x->output_state = suffixState + 1;
        return x->states[suffixState].transitionElement;
    }
    
    double n = (double)rand() / (double)((unsigned)RAND_MAX + 1);
    
    if ((n >= x->probability) && (x->states[x->output_state].suffixLink != 0))
    {
        long suffixState = x->states[x->output_state].suffixLink;
        x->output_state = suffixState + 1;
        return x->states[suffixState].transitionElement;
    }
    else
    {
        double nn = n * x->states[x->output_state].numberOfTransitionElements;
        for (long i = 0; i <= x->states[x->output_state].numberOfTransitionElements; i++)
        {
            if (((double)i <= nn) && (nn < ((double)i + 1.0)))
            {
                x->output_state = x->states[x->output_state].transitionEndStates[i];
                break;
            }
        }
        return x->states[x->output_state - 1].transitionElement;
    }
}
