using System;
using System.Collections.Generic;

namespace LMS.Models.LMSModels;

public partial class Professor
{
    public string Id { get; set; } = null!;

    public string First { get; set; } = null!;

    public string Last { get; set; } = null!;

    public DateOnly Dob { get; set; }

    public string Dept { get; set; } = null!;

    public virtual ICollection<Class> Classes { get; set; } = new List<Class>();

    public virtual Department DeptNavigation { get; set; } = null!;
}
