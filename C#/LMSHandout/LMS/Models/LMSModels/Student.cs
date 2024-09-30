using System;
using System.Collections.Generic;

namespace LMS.Models.LMSModels;

public partial class Student
{
    public string Id { get; set; } = null!;

    public string First { get; set; } = null!;

    public string Last { get; set; } = null!;

    public DateOnly Dob { get; set; }

    public string Major { get; set; } = null!;

    public virtual ICollection<Enrollment> Enrollments { get; set; } = new List<Enrollment>();

    public virtual Department MajorNavigation { get; set; } = null!;

    public virtual ICollection<Submission> Submissions { get; set; } = new List<Submission>();
}
