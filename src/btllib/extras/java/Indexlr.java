/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package btllib;

public class Indexlr {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected Indexlr(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(Indexlr obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        btllibJNI.delete_Indexlr(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  static public class Flag {
    private transient long swigCPtr;
    protected transient boolean swigCMemOwn;
  
    protected Flag(long cPtr, boolean cMemoryOwn) {
      swigCMemOwn = cMemoryOwn;
      swigCPtr = cPtr;
    }
  
    protected static long getCPtr(Flag obj) {
      return (obj == null) ? 0 : obj.swigCPtr;
    }
  
    @SuppressWarnings("deprecation")
    protected void finalize() {
      delete();
    }
  
    public synchronized void delete() {
      if (swigCPtr != 0) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          btllibJNI.delete_Indexlr_Flag(swigCPtr);
        }
        swigCPtr = 0;
      }
    }
  
    public Flag() {
      this(btllibJNI.new_Indexlr_Flag(), true);
    }
  
    public final static long ID = btllibJNI.Indexlr_Flag_ID_get();
    public final static long NO_ID = btllibJNI.Indexlr_Flag_NO_ID_get();
    public final static long BX = btllibJNI.Indexlr_Flag_BX_get();
    public final static long NO_BX = btllibJNI.Indexlr_Flag_NO_BX_get();
    public final static long SEQ = btllibJNI.Indexlr_Flag_SEQ_get();
    public final static long NO_SEQ = btllibJNI.Indexlr_Flag_NO_SEQ_get();
    public final static long FILTER_IN = btllibJNI.Indexlr_Flag_FILTER_IN_get();
    public final static long NO_FILTER_IN = btllibJNI.Indexlr_Flag_NO_FILTER_IN_get();
    public final static long FILTER_OUT = btllibJNI.Indexlr_Flag_FILTER_OUT_get();
    public final static long NO_FILTER_OUT = btllibJNI.Indexlr_Flag_NO_FILTER_OUT_get();
    public final static long SHORT_MODE = btllibJNI.Indexlr_Flag_SHORT_MODE_get();
    public final static long LONG_MODE = btllibJNI.Indexlr_Flag_LONG_MODE_get();
  }

  public boolean output_id() {
    return btllibJNI.Indexlr_output_id(swigCPtr, this);
  }

  public boolean output_bx() {
    return btllibJNI.Indexlr_output_bx(swigCPtr, this);
  }

  public boolean output_seq() {
    return btllibJNI.Indexlr_output_seq(swigCPtr, this);
  }

  public boolean filter_in() {
    return btllibJNI.Indexlr_filter_in(swigCPtr, this);
  }

  public boolean filter_out() {
    return btllibJNI.Indexlr_filter_out(swigCPtr, this);
  }

  public boolean short_mode() {
    return btllibJNI.Indexlr_short_mode(swigCPtr, this);
  }

  public boolean long_mode() {
    return btllibJNI.Indexlr_long_mode(swigCPtr, this);
  }

  static public class Read {
    private transient long swigCPtr;
    protected transient boolean swigCMemOwn;
  
    protected Read(long cPtr, boolean cMemoryOwn) {
      swigCMemOwn = cMemoryOwn;
      swigCPtr = cPtr;
    }
  
    protected static long getCPtr(Read obj) {
      return (obj == null) ? 0 : obj.swigCPtr;
    }
  
    @SuppressWarnings("deprecation")
    protected void finalize() {
      delete();
    }
  
    public synchronized void delete() {
      if (swigCPtr != 0) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          btllibJNI.delete_Indexlr_Read(swigCPtr);
        }
        swigCPtr = 0;
      }
    }
  
    public Read() {
      this(btllibJNI.new_Indexlr_Read__SWIG_0(), true);
    }
  
    public Read(long num, String id, String comment, String seq) {
      this(btllibJNI.new_Indexlr_Read__SWIG_1(num, id, comment, seq), true);
    }
  
    public void setNum(long value) {
      btllibJNI.Indexlr_Read_num_set(swigCPtr, this, value);
    }
  
    public long getNum() {
      return btllibJNI.Indexlr_Read_num_get(swigCPtr, this);
    }
  
    public void setId(String value) {
      btllibJNI.Indexlr_Read_id_set(swigCPtr, this, value);
    }
  
    public String getId() {
      return btllibJNI.Indexlr_Read_id_get(swigCPtr, this);
    }
  
    public void setComment(String value) {
      btllibJNI.Indexlr_Read_comment_set(swigCPtr, this, value);
    }
  
    public String getComment() {
      return btllibJNI.Indexlr_Read_comment_get(swigCPtr, this);
    }
  
    public void setSeq(String value) {
      btllibJNI.Indexlr_Read_seq_set(swigCPtr, this, value);
    }
  
    public String getSeq() {
      return btllibJNI.Indexlr_Read_seq_get(swigCPtr, this);
    }
  
  }

  static public class Minimizer {
    private transient long swigCPtr;
    protected transient boolean swigCMemOwn;
  
    protected Minimizer(long cPtr, boolean cMemoryOwn) {
      swigCMemOwn = cMemoryOwn;
      swigCPtr = cPtr;
    }
  
    protected static long getCPtr(Minimizer obj) {
      return (obj == null) ? 0 : obj.swigCPtr;
    }
  
    @SuppressWarnings("deprecation")
    protected void finalize() {
      delete();
    }
  
    public synchronized void delete() {
      if (swigCPtr != 0) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          btllibJNI.delete_Indexlr_Minimizer(swigCPtr);
        }
        swigCPtr = 0;
      }
    }
  
    public Minimizer() {
      this(btllibJNI.new_Indexlr_Minimizer__SWIG_0(), true);
    }
  
    public Minimizer(SWIGTYPE_p_uint64_t min_hash, SWIGTYPE_p_uint64_t out_hash, long pos, boolean forward, String seq) {
      this(btllibJNI.new_Indexlr_Minimizer__SWIG_1(SWIGTYPE_p_uint64_t.getCPtr(min_hash), SWIGTYPE_p_uint64_t.getCPtr(out_hash), pos, forward, seq), true);
    }
  
    public void setMin_hash(SWIGTYPE_p_uint64_t value) {
      btllibJNI.Indexlr_Minimizer_min_hash_set(swigCPtr, this, SWIGTYPE_p_uint64_t.getCPtr(value));
    }
  
    public SWIGTYPE_p_uint64_t getMin_hash() {
      return new SWIGTYPE_p_uint64_t(btllibJNI.Indexlr_Minimizer_min_hash_get(swigCPtr, this), true);
    }
  
    public void setOut_hash(SWIGTYPE_p_uint64_t value) {
      btllibJNI.Indexlr_Minimizer_out_hash_set(swigCPtr, this, SWIGTYPE_p_uint64_t.getCPtr(value));
    }
  
    public SWIGTYPE_p_uint64_t getOut_hash() {
      return new SWIGTYPE_p_uint64_t(btllibJNI.Indexlr_Minimizer_out_hash_get(swigCPtr, this), true);
    }
  
    public void setPos(long value) {
      btllibJNI.Indexlr_Minimizer_pos_set(swigCPtr, this, value);
    }
  
    public long getPos() {
      return btllibJNI.Indexlr_Minimizer_pos_get(swigCPtr, this);
    }
  
    public void setForward(boolean value) {
      btllibJNI.Indexlr_Minimizer_forward_set(swigCPtr, this, value);
    }
  
    public boolean getForward() {
      return btllibJNI.Indexlr_Minimizer_forward_get(swigCPtr, this);
    }
  
    public void setSeq(String value) {
      btllibJNI.Indexlr_Minimizer_seq_set(swigCPtr, this, value);
    }
  
    public String getSeq() {
      return btllibJNI.Indexlr_Minimizer_seq_get(swigCPtr, this);
    }
  
  }

  static public class Record {
    private transient long swigCPtr;
    protected transient boolean swigCMemOwn;
  
    protected Record(long cPtr, boolean cMemoryOwn) {
      swigCMemOwn = cMemoryOwn;
      swigCPtr = cPtr;
    }
  
    protected static long getCPtr(Record obj) {
      return (obj == null) ? 0 : obj.swigCPtr;
    }
  
    @SuppressWarnings("deprecation")
    protected void finalize() {
      delete();
    }
  
    public synchronized void delete() {
      if (swigCPtr != 0) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          btllibJNI.delete_Indexlr_Record(swigCPtr);
        }
        swigCPtr = 0;
      }
    }
  
    public Record() {
      this(btllibJNI.new_Indexlr_Record__SWIG_0(), true);
    }
  
    public Record(long num, String id, String barcode, SWIGTYPE_p_std__vectorT_btllib__Indexlr__Minimizer_t minimizers) {
      this(btllibJNI.new_Indexlr_Record__SWIG_1(num, id, barcode, SWIGTYPE_p_std__vectorT_btllib__Indexlr__Minimizer_t.getCPtr(minimizers)), true);
    }
  
    public void setNum(long value) {
      btllibJNI.Indexlr_Record_num_set(swigCPtr, this, value);
    }
  
    public long getNum() {
      return btllibJNI.Indexlr_Record_num_get(swigCPtr, this);
    }
  
    public void setId(String value) {
      btllibJNI.Indexlr_Record_id_set(swigCPtr, this, value);
    }
  
    public String getId() {
      return btllibJNI.Indexlr_Record_id_get(swigCPtr, this);
    }
  
    public void setBarcode(String value) {
      btllibJNI.Indexlr_Record_barcode_set(swigCPtr, this, value);
    }
  
    public String getBarcode() {
      return btllibJNI.Indexlr_Record_barcode_get(swigCPtr, this);
    }
  
    public void setMinimizers(SWIGTYPE_p_std__vectorT_btllib__Indexlr__Minimizer_t value) {
      btllibJNI.Indexlr_Record_minimizers_set(swigCPtr, this, SWIGTYPE_p_std__vectorT_btllib__Indexlr__Minimizer_t.getCPtr(value));
    }
  
    public SWIGTYPE_p_std__vectorT_btllib__Indexlr__Minimizer_t getMinimizers() {
      long cPtr = btllibJNI.Indexlr_Record_minimizers_get(swigCPtr, this);
      return (cPtr == 0) ? null : new SWIGTYPE_p_std__vectorT_btllib__Indexlr__Minimizer_t(cPtr, false);
    }
  
  }

  public Indexlr.Record get_minimizers() {
    return new Indexlr.Record(btllibJNI.Indexlr_get_minimizers(swigCPtr, this), true);
  }

  public Indexlr(String seqfile, long k, long w, long flags, long threads, boolean verbose, BloomFilter bf1, BloomFilter bf2) {
    this(btllibJNI.new_Indexlr__SWIG_0(seqfile, k, w, flags, threads, verbose, BloomFilter.getCPtr(bf1), bf1, BloomFilter.getCPtr(bf2), bf2), true);
  }

  public Indexlr(String seqfile, long k, long w, long flags, long threads, boolean verbose, BloomFilter bf1) {
    this(btllibJNI.new_Indexlr__SWIG_1(seqfile, k, w, flags, threads, verbose, BloomFilter.getCPtr(bf1), bf1), true);
  }

  public Indexlr(String seqfile, long k, long w, long flags, long threads, boolean verbose) {
    this(btllibJNI.new_Indexlr__SWIG_2(seqfile, k, w, flags, threads, verbose), true);
  }

  public Indexlr(String seqfile, long k, long w, long flags, long threads) {
    this(btllibJNI.new_Indexlr__SWIG_3(seqfile, k, w, flags, threads), true);
  }

  public Indexlr(String seqfile, long k, long w, long flags) {
    this(btllibJNI.new_Indexlr__SWIG_4(seqfile, k, w, flags), true);
  }

  public Indexlr(String seqfile, long k, long w) {
    this(btllibJNI.new_Indexlr__SWIG_5(seqfile, k, w), true);
  }

  public final static long MAX_SIMULTANEOUS_INDEXLRS = btllibJNI.Indexlr_MAX_SIMULTANEOUS_INDEXLRS_get();
  public final static long SHORT_MODE_BUFFER_SIZE = btllibJNI.Indexlr_SHORT_MODE_BUFFER_SIZE_get();
  public final static long SHORT_MODE_BLOCK_SIZE = btllibJNI.Indexlr_SHORT_MODE_BLOCK_SIZE_get();
  public final static long LONG_MODE_BUFFER_SIZE = btllibJNI.Indexlr_LONG_MODE_BUFFER_SIZE_get();
  public final static long LONG_MODE_BLOCK_SIZE = btllibJNI.Indexlr_LONG_MODE_BLOCK_SIZE_get();
}
