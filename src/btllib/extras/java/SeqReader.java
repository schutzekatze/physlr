/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package btllib;

public class SeqReader {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected SeqReader(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(SeqReader obj) {
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
        btllibJNI.delete_SeqReader(swigCPtr);
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
          btllibJNI.delete_SeqReader_Flag(swigCPtr);
        }
        swigCPtr = 0;
      }
    }
  
    public Flag() {
      this(btllibJNI.new_SeqReader_Flag(), true);
    }
  
    public final static long FOLD_CASE = btllibJNI.SeqReader_Flag_FOLD_CASE_get();
    public final static long NO_FOLD_CASE = btllibJNI.SeqReader_Flag_NO_FOLD_CASE_get();
    public final static long NO_TRIM_MASKED = btllibJNI.SeqReader_Flag_NO_TRIM_MASKED_get();
    public final static long TRIM_MASKED = btllibJNI.SeqReader_Flag_TRIM_MASKED_get();
  }

  public SeqReader(String source_path, long flags, long threads, long buffer_size, long block_size) {
    this(btllibJNI.new_SeqReader__SWIG_0(source_path, flags, threads, buffer_size, block_size), true);
  }

  public SeqReader(String source_path, long flags, long threads, long buffer_size) {
    this(btllibJNI.new_SeqReader__SWIG_1(source_path, flags, threads, buffer_size), true);
  }

  public SeqReader(String source_path, long flags, long threads) {
    this(btllibJNI.new_SeqReader__SWIG_2(source_path, flags, threads), true);
  }

  public SeqReader(String source_path, long flags) {
    this(btllibJNI.new_SeqReader__SWIG_3(source_path, flags), true);
  }

  public SeqReader(String source_path) {
    this(btllibJNI.new_SeqReader__SWIG_4(source_path), true);
  }

  public void close() {
    btllibJNI.SeqReader_close(swigCPtr, this);
  }

  public boolean fold_case() {
    return btllibJNI.SeqReader_fold_case(swigCPtr, this);
  }

  public boolean trim_masked() {
    return btllibJNI.SeqReader_trim_masked(swigCPtr, this);
  }

  public SeqReader.Format get_format() {
    return SeqReader.Format.swigToEnum(btllibJNI.SeqReader_get_format(swigCPtr, this));
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
          btllibJNI.delete_SeqReader_Record(swigCPtr);
        }
        swigCPtr = 0;
      }
    }
  
    public void setNum(long value) {
      btllibJNI.SeqReader_Record_num_set(swigCPtr, this, value);
    }
  
    public long getNum() {
      return btllibJNI.SeqReader_Record_num_get(swigCPtr, this);
    }
  
    public void setName(String value) {
      btllibJNI.SeqReader_Record_name_set(swigCPtr, this, value);
    }
  
    public String getName() {
      return btllibJNI.SeqReader_Record_name_get(swigCPtr, this);
    }
  
    public void setComment(String value) {
      btllibJNI.SeqReader_Record_comment_set(swigCPtr, this, value);
    }
  
    public String getComment() {
      return btllibJNI.SeqReader_Record_comment_get(swigCPtr, this);
    }
  
    public void setSeq(String value) {
      btllibJNI.SeqReader_Record_seq_set(swigCPtr, this, value);
    }
  
    public String getSeq() {
      return btllibJNI.SeqReader_Record_seq_get(swigCPtr, this);
    }
  
    public void setQual(String value) {
      btllibJNI.SeqReader_Record_qual_set(swigCPtr, this, value);
    }
  
    public String getQual() {
      return btllibJNI.SeqReader_Record_qual_get(swigCPtr, this);
    }
  
    public Record() {
      this(btllibJNI.new_SeqReader_Record(), true);
    }
  
  }

  public SeqReader.Record read() {
    return new SeqReader.Record(btllibJNI.SeqReader_read(swigCPtr, this), true);
  }

  public final static class Format {
    public final static SeqReader.Format UNDETERMINED = new SeqReader.Format("UNDETERMINED");
    public final static SeqReader.Format FASTA = new SeqReader.Format("FASTA");
    public final static SeqReader.Format FASTQ = new SeqReader.Format("FASTQ");
    public final static SeqReader.Format SAM = new SeqReader.Format("SAM");
    public final static SeqReader.Format GFA2 = new SeqReader.Format("GFA2");
    public final static SeqReader.Format INVALID = new SeqReader.Format("INVALID");

    public final int swigValue() {
      return swigValue;
    }

    public String toString() {
      return swigName;
    }

    public static Format swigToEnum(int swigValue) {
      if (swigValue < swigValues.length && swigValue >= 0 && swigValues[swigValue].swigValue == swigValue)
        return swigValues[swigValue];
      for (int i = 0; i < swigValues.length; i++)
        if (swigValues[i].swigValue == swigValue)
          return swigValues[i];
      throw new IllegalArgumentException("No enum " + Format.class + " with value " + swigValue);
    }

    private Format(String swigName) {
      this.swigName = swigName;
      this.swigValue = swigNext++;
    }

    private Format(String swigName, int swigValue) {
      this.swigName = swigName;
      this.swigValue = swigValue;
      swigNext = swigValue+1;
    }

    private Format(String swigName, Format swigEnum) {
      this.swigName = swigName;
      this.swigValue = swigEnum.swigValue;
      swigNext = this.swigValue+1;
    }

    private static Format[] swigValues = { UNDETERMINED, FASTA, FASTQ, SAM, GFA2, INVALID };
    private static int swigNext = 0;
    private final int swigValue;
    private final String swigName;
  }

  public final static long MAX_SIMULTANEOUS_SEQREADERS = btllibJNI.SeqReader_MAX_SIMULTANEOUS_SEQREADERS_get();
}
