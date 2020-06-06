import ctypes,sys
from ctypes.util import find_library

user_callback = None

DLT_EN10MB = 1

def mycallback(us,h,data):
    header = pcap_pkthdr ()
    header.len = h[0].len
    header.caplen = h[0].caplen
    header.ts = timeval(h[0].tv_sec,h[0].tv_usec)
    if user_callback is not None:
        user_callback (us,header,bytearray(data[:header.caplen]))



pcap = ctypes.cdll.LoadLibrary("libpcap.so")


class timeval():
    def __init__(self,tv_sec,tv_usec):
        self.tv_sec = tv_sec
        self.tv_usec = tv_usec

class pcap_pkthdr():
    def __init__(self):
        self.len=0
        self.caplen=0
        self.ts=timeval(0,0)

class pcappkthdr(ctypes.Structure):
    _fields_ = [("tv_sec", ctypes.c_long), ("tv_usec", ctypes.c_long), ("caplen", ctypes.c_uint32), ("len", ctypes.c_uint32)]


def pcap_open_offline(fname,errbuf):
    #pcap_t *pcap_open_offline(const char *fname, char *errbuf);
    poo = pcap.pcap_open_offline
    fn =  bytes(str(fname), 'ascii')
    poo.restype = ctypes.POINTER(ctypes.c_void_p)
    eb = ctypes.create_string_buffer(256)
    handle = poo(fn,eb)
    errbuf.extend(bytes(format(eb.value).encode('ascii')))
    return handle


def pcap_open_dead(linktype,snaplen):
    #pcap_t *pcap_open_dead(int linktype, int snaplen)
    pod = pcap.pcap_open_dead
    pod.restype = ctypes.POINTER(ctypes.c_void_p)
    lt = ctypes.c_int(linktype)
    sn = ctypes.c_int(snaplen)
    handle = pod(lt,sn)
    return handle

def pcap_dump_open(descr, fname):
    #pcap_dumper_t *pcap_dump_open(pcap_t *p, const char *fname);
    pdo = pcap.pcap_dump_open
    pdo.restype = ctypes.POINTER(ctypes.c_void_p)
    ds = ctypes.POINTER(ctypes.c_void_p(descr))
    fn =  bytes(str(fname), 'ascii')
    handle = pdo(ds,fn)
    return handle

def pcap_dump(dumper,header,data):
    # void pcap_dump(u_char *user, struct pcap_pkthdr *h,u_char *sp);
    pd = pcap.pcap_dump
    dp = ctypes.c_void_p(dumper)
    haux = pcappkthdr()
    haux.len = header.len
    haux.caplen = header.caplen
    haux.tv_sec = header.ts.tv_sec
    haux.tv_usec = header.ts.tv_usec
    h = ctypes.byref(haux)
    d = ctypes.c_char_p(bytes(data))
    pd(dp,h,d)



def pcap_open_live(device,snaplen,promisc,to_ms,errbuf):
    
    #pcap_t *pcap_open_live(const char *device, int snaplen,int promisc, int to_ms, char *errbuf)
    pol = pcap.pcap_open_live
    pol.restype = ctypes.POINTER(ctypes.c_void_p)
    dv =  bytes(str(device), 'ascii')
    sn = ctypes.c_int(snaplen)
    tms = ctypes.c_int(to_ms)
    pr = ctypes.c_int(promisc)
    eb = ctypes.create_string_buffer(256)
    handle = pol(dv,sn,pr,tms,eb)
    errbuf.extend(bytes(format(eb.value).encode('ascii')))

    return handle

def pcap_close(handle):
    #void pcap_close(pcap_t *p);
    pc = pcap.pcap_close
    pc(handle)

def pcap_dump_close(handle):
    #void pcap_close(pcap_dumper_t *p);
    pdc = pcap.pcap_dump_close
    pdc(handle)

def pcap_next(handle,header):
    #const u_char *pcap_next(pcap_t *p, struct pcap_pkthdr *h)
    pn = pcap.pcap_next
    pn.restype = ctypes.c_char_p
    h = pcappkthdr()
    aux = pn(handle,ctypes.byref(h))
    header.len = h.len
    header.caplen = h.caplen
    header.ts = timeval(h.tv_sec,h.tv_usec)
    return bytearray(aux)


def pcap_loop(handle,cnt,callback_fun,user):
    global user_callback
    user_callback = callback_fun
    #  typedef void (*pcap_handler)(u_char *user, const struct pcap_pkthdr *h,const u_char *bytes);
    PCAP_HANDLER = ctypes.CFUNCTYPE(ctypes.c_void_p, ctypes.c_char_p,ctypes.POINTER(pcappkthdr),ctypes.POINTER(ctypes.c_uint8))
    cf = PCAP_HANDLER(mycallback)
    #int pcap_loop(pcap_t *p, int cnt,pcap_handler callback, u_char *user);
    pl = pcap.pcap_loop
    pl.restype = ctypes.c_int
    us = ctypes.c_void_p(user)
    c = ctypes.c_int(cnt)
    ret = pl(handle,c,cf,us)
    user_callback = None
    return ret
def pcap_dispatch(handle,cnt,callback_fun,user):
    global user_callback
    user_callback = callback_fun
    #  typedef void (*pcap_handler)(u_char *user, const struct pcap_pkthdr *h,const u_char *bytes);
    PCAP_HANDLER = ctypes.CFUNCTYPE(ctypes.c_void_p, ctypes.c_char_p,ctypes.POINTER(pcappkthdr),ctypes.POINTER(ctypes.c_uint8))
    cf = PCAP_HANDLER(mycallback)
    #int pcap_loop(pcap_t *p, int cnt,pcap_handler callback, u_char *user);
    pd = pcap.pcap_dispatch
    pd.restype = ctypes.c_int
    us = ctypes.c_void_p(user)
    c = ctypes.c_int(cnt)
    ret = pd(handle,c,cf,us)
    user_callback = None
    return ret
def pcap_breakloop(hanlde):
    #void pcap_breakloop(pcap_t *);
    pbl = pcap.pcap_breakloop
    pbl(hanlde)

def pcap_inject(handle,buf,size):
    #int pcap_inject(pcap_t *p, const void *buf, size_t size);
    pi = pcap.pcap_inject
    pi.restype = ctypes.c_int
    ret = pi(handle,ctypes.c_char_p(buf),ctypes.c_longlong(size))
    return ret





